'''
@file: CMake.py
@date: Okt 17, 2012

@brief: Support for SCons to unpack, cmake and make tarballs.
@version: 3.0 

@author: Milos Subotic milos.subotic.sm@gmail.com
@license: MIT
'''

###############################################################################

from __future__ import print_function

import os
import sys
import shutil
import copy

from SCons.Script import *

from ShellHelpers import *
import Make
import Tarball
import Dirs
import Targets

###############################################################################

class CMakeWarning(SCons.Warnings.Warning):
	pass

class CMakeNotFound(CMakeWarning):
	pass

class NotSupportedCMakeBuildType(CMakeWarning):
	pass

SCons.Warnings.enableWarningClass(CMakeWarning)

def _detectCMake(env):
	''' Try to detect the "cmake"'''
	try:
		return env['CMAKE']
	except KeyError:
		pass

	tar = env.WhereIs('cmake')
	if tar:
		return tar

	raise SCons.Errors.StopError(CMakeNotFound,	'Could not detect "cmake"')
	return None

def _CMake(env, dirs, unpacked, compileParams, cmakeFlags, build):
	# FIXME Hardcoded platformLevel.
	platformLevel = '14'

	cmakeCmd = '$CMAKE -H"{unpack}" -B"{build}" '\
			'-DCMAKE_INSTALL_PREFIX="{prefix}"'\
			.format(unpack = dirs.projectUnpackDir.abspath,
					build = dirs.projectBuildDir.abspath,
					prefix = dirs.projectOutDir.abspath)

	if compileParams.platform == 'android':
		cmakeCmd += ' -DANDROID_NDK="{ndk}" '\
			'-DCMAKE_TOOLCHAIN_FILE="{toolchainFile}" '\
			'-DANDROID_API_LEVEL={platformLevel} -DANDROID_ABI={abi}'\
			.format(ndk = env['ANDROID_NDK'],
					toolchainFile = Dir('#').Dir('site_scons')\
						.File('android.toolchain.cmake').abspath, 
					platformLevel = platformLevel, 
					abi = compileParams.abi)
	
	if build == 'debug':
		build = 'Debug'
	elif build == 'release':
		build = 'Release'
	else:
		# Possible CMake build types: 
		# None, Debug, Release, RelWithDebInfo, MinSizeRel, Profile
		raise SCons.Errors.StopError(NotSupportedCMakeBuildType, 
			'Build type "{0}" is not supported.'.format(build))
	cmakeCmd += ' -DCMAKE_BUILD_TYPE={0}'.format(build)
	
	cmakeCmd += ' ' + cmakeFlags

	actions = []
	# If unpack and build dirs are same do not delete build dir.
	if dirs.projectUnpackDir != dirs.projectBuildDir:
		actions += [
				Delete(dirs.projectBuildDir),
				Mkdir(dirs.projectBuildDir)
		]
		
	actions += [ 
			cmakeCmd,
			Mkdir(dirs.cmaked)
	]		
	
	return [env.Command(target = dirs.cmaked,
			source = unpacked,
			action = actions)]
	
def addMethodCMake(env):
	env['CMAKE'] = _detectCMake(env)
	env.AddMethod(_CMake, "CMake")

###############################################################################

class CMake:
	
	def clone(self):
		return copy.copy(self)
		
	def dirs(self, env, projectName, statusDir, unpackDir, compileParams):
		return Dirs.Dirs(env = env, 
				projectName = projectName, 
				statusDir = statusDir, 
				unpackDir = unpackDir, 
				platformAbiBuild = compileParams.platformAbiBuild)
		
	def unpack(self, env, allBuildsEnv, dirs, tarball, dirInTarball, 
				prerequisite):
		return allBuildsEnv.Unpack(dirs = dirs,
				tarball = tarball,
				dirInTarball = dirInTarball,
				prerequisite = prerequisite)
			
	def CMakeFlags(self, env, cmakeFlags, compileParams):
		return cmakeFlags
			
	def CMake(self, env, dirs, unpacked, cmakeFlags, compileParams):
		cmakeFlags = self.CMakeFlags(env = env,
			cmakeFlags = cmakeFlags,
			compileParams = compileParams)
			
		return env.CMake(
			dirs = dirs,
			unpacked = unpacked,
			compileParams = compileParams,
			cmakeFlags = cmakeFlags,
			build = compileParams.build)	
		
	def makeFlags(self, env, compileParams):
		if env['VERBOSE']:
			makeFlags = 'V=1'
		else:
			makeFlags = 'V=""'
		return makeFlags
		
	def makeAll(self, env, dirs, prerequisite, flags):
		return env.Make(statusFile = dirs.compiled, 
				makeTarget = 'all', 
				prerequisite = prerequisite,
				buildDir = dirs.projectBuildDir,
				flags = flags)
				
	def makeInstall(self, env, dirs, prerequisite, flags):
		return env.Make(statusFile = dirs.installed, 
				makeTarget = 'install', 
				prerequisite = prerequisite,
				buildDir = dirs.projectBuildDir,
				flags = flags)
				
	def make(self, env, dirs, cmaked, compileParams):
		makeFlags = self.makeFlags(env = env, 
				compileParams = compileParams)
	
		compiled = self.makeAll(env = env,
				dirs = dirs,
				prerequisite = cmaked,
				flags = makeFlags)
	
		return self.makeInstall(env = env,
				dirs = dirs,
				prerequisite = compiled,
				flags = makeFlags)
				
	def makeTargets(self, env, dirs, cmaked, compileParams, targets):
		makeFlags = self.makeFlags(env = env, 
				compileParams = compileParams)
				
		targetsForReturn = []
		for target in targets:

			targetsForReturn += env.Make(statusFile = None, 
					makeTarget = target, 
					prerequisite = cmaked,
					buildDir = dirs.projectBuildDir,
					flags = makeFlags)	
					
		return targetsForReturn
				
	def CMakeMake(self, 
			env, 
			allBuildsEnv, 
			compileParams,
			projectName, 
			tarball, 
			dirInTarball = None,
			statusDir = None,
			unpackDir = None,
			targets = None,
			cmakeFlags = '',
			prerequisite = None):
		
		###################################
		# Add necessary methods.
		
		for e in [env, allBuildsEnv]:
			Tarball.addMethodUnpack(e)
			addMethodCMake(e)
			Make.addMethodMake(e)
					
		###############################
		# Check some crucial variables.
		
		if not unpackDir:
			unpackDir = Dir(Tarball.defaultUnpackDir)
		if not statusDir:
			statusDir = Dir(Dirs.defaultStatusDir)
			
		###############################
		# Dirs.

		dirs = self.dirs(env = env, 
				projectName = projectName, 
				statusDir = statusDir, 
				unpackDir = unpackDir, 
				compileParams = compileParams)
				
		###############################
		# Add for distclean.
		
		env.addForDistclean([
			dirs.unpackedStatusDir,
			dirs.projectStatusDir,
			dirs.projectUnpackDir,
			dirs.projectBuildDir,
			dirs.projectOutDir
		])	
		
		###############################
		# Unpack.

		unpacked = self.unpack(env = env, 
				allBuildsEnv = allBuildsEnv, 
				dirs = dirs,
				tarball = tarball,
				dirInTarball = dirInTarball,
				prerequisite = None)
				
		###############################
		# CMake.

		if prerequisite:
			unpacked += prerequisite

		cmaked = self.CMake(
				env = env,
				dirs = dirs,
				unpacked = unpacked,
				cmakeFlags = cmakeFlags,
				compileParams = compileParams)
		
		###############################
		# Do targets


		# If targets are not specified do standard all and install targets.
		if not targets:

			return self.make(env = env, 
					dirs = dirs, 
					cmaked = cmaked,
					compileParams = compileParams)
		
		else:
			# TODO This code is not tested.
			if targets:
				if not SCons.Util.is_List(targets):
					targets = [targets]
					
			return self.make(env = env, 
					dirs = dirs, 
					cmaked = cmaked,
					compileParams = compileParams,
					targets = targets)

###############################################################################

def crossCMakeMake(env, 
		cmake,
		projectName, 
		tarball, 
		dirInTarball = None,
		unpackDir = None,
		statusDir = None,
		targets = None,
		cmakeFlags = '',
		targetFilter = None,
		prerequisite = None):
	
	###################################
	
	def _buildFunction(compileParams):
		buildCmake = cmake.clone()
		return buildCmake.CMakeMake(env = compileParams.clonedEnv,
						allBuildsEnv = compileParams.originalEnv,
						compileParams = compileParams,
						projectName = projectName,
						tarball = tarball,
						unpackDir = unpackDir,
						statusDir = statusDir,
						targets = targets,
						cmakeFlags = cmakeFlags,
						prerequisite = prerequisite)
						
	###################################

	targets = Targets.buildCrossCompileTargets(env = env,
						projectName = projectName,
						buildFunction = _buildFunction,
						targetFilter = targetFilter)
						
	return targets

###############################################################################

