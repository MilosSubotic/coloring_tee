'''
@file: ConfigureMake.py
@date: Okt 17, 2012

@brief: Support for SCons to unpack, configure and make tarballs.
@version: 1.0 

@author: Milos Subotic milos.subotic.sm@gmail.com
@license: MIT
'''

###############################################################################

from __future__ import print_function

import os
import sys
import shutil

from SCons.Script import *

from ShellHelpers import *
from Make import *

import Targets

###############################################################################

defaultUnpackDir = './unpack'
defaultStatusDir = './status'

class TarWarning(SCons.Warnings.Warning):
	pass

class TarNotFound(TarWarning):
	pass

SCons.Warnings.enableWarningClass(TarWarning)

def _detectTar(env):
	''' Try to detect the "tar"'''
	try:
		return env['TAR']
	except KeyError:
		pass

	tar = env.WhereIs('tar')
	if tar:
		return tar

	raise SCons.Errors.StopError(
		TarNotFound,
		'Could not detect "tar"')
	return None
	
def _Unpack(env, unpacked, projectUnpackDir, unpackDir, tarball):
		
	return [env.Command(target = unpacked,
			source = None,
			action = [ 
				Mkdir(unpackDir),
				Delete(projectUnpackDir),
				Action('$TAR xfv {0} -C {1}'.format(tarball, unpackDir)),
				Mkdir(unpacked)
			])]


def _addMethodUnpack(env):
	env['TAR'] = _detectTar(env)

	env.AddMethod(_Unpack, "Unpack")

def _Configure(env, configured, unpacked, projectOutDir, 
		projectBuildDir, projectUnpackDir, configureFlags, enableDebug):
		
	configure = projectUnpackDir.File('configure')
	
	configureCmd = 'cd ' + projectBuildDir.abspath + '; ' 
	configureCmd += configure.abspath + ' ' + configureFlags
	if enableDebug:
		configureCmd += ' --enable-debug'
	configureCmd += ' --prefix=' + projectOutDir.abspath
	
	return [env.Command(target = configured,
			source = unpacked,
			action = [ 
				Delete(projectBuildDir),
				Mkdir(projectBuildDir),
				configureCmd,
				Mkdir(configured)
			])]
	
def _addMethodConfigure(env):
	env.AddMethod(_Configure, "Configure")

###############################################################################

def configureMake(env, unpackEnv, projectName, tarball, 
				compileParams,
				statusDir = None,
				unpackDir = None,
				enableDebug = True, targets = None,
				configureFlags = '',
				platform = '', abi = ''):

	###################################
	# Set variables.

	if targets:
		if not SCons.Util.is_List(targets):
			targets = [targets]

	enableDebug = compileParams.build == 'debug'

	if not unpackDir:
		unpackDir = Dir(defaultUnpackDir)
	if not statusDir:
		statusDir = Dir(defaultStatusDir)
		
	unpackedStatusDir = statusDir.Dir(projectName)
	projectStatusDir = statusDir.Dir(projectName).Dir(
			compileParams.platformAbiBuild)
	projectBuildDir = env.getVPATH().Dir(projectName).Dir(
			compileParams.platformAbiBuild)

	projectOutDir = env.getPREFIX().Dir(projectName).Dir(
			compileParams.platformAbiBuild)


	###################################
	# Targets.
	
	unpacked = unpackedStatusDir.Dir('unpacked')
	configured = projectStatusDir.Dir('configured')
	compiled = projectStatusDir.Dir('compiled')
	installed = projectStatusDir.Dir('installed')

	###################################
	# Unpack.

	projectUnpackDir = unpackDir.Dir(projectName)

	nextSource = unpackEnv.Unpack(unpacked = unpacked, 
			projectUnpackDir = projectUnpackDir,
			unpackDir = unpackDir,
			tarball = tarball
			)
			
	###################################
	# Configure.
		
	nextSource = env.Configure(configured = configured, 
		unpacked = nextSource, 
		projectOutDir = projectOutDir, 
		projectBuildDir = projectBuildDir, 
		projectUnpackDir = projectUnpackDir, 
		configureFlags = configureFlags,
		enableDebug = enableDebug)
		
	###########################################################################
	# Do targets

	# If targets are not specified do standard all and install targets.
	if not targets:

		nextSource = env.Make(statusFile = compiled, 
					makeTarget = 'all', 
					prerequisite = nextSource,
					buildDir = projectBuildDir
					)
		nextSource = env.Make(statusFile = installed, 
					makeTarget = 'install', 
					prerequisite = nextSource,
					buildDir = projectBuildDir
					)	

		return nextSource
		
	else:
		# TODO This code is not tested.
		
		targetsForReturn = []
		for target in targets:

			targetsForReturn += env.Make(statusFile = None, 
					makeTarget = target, 
					prerequisite = nextSource,
					buildDir = projectBuildDir
					)	
					
		return targetsForReturn

###############################################################################

class ConfigureParams:
	def __init__(self, env, platform, abi, build):
		self.env = env
		self.platform = platform
		self.abi = abi
		self.build = build
		self.platformAbiBuild = os.path.join(platform, abi, build)

###############################################################################

def crossConfigureMake(env, projectName, tarball, unpackDir = None,
				statusDir = None,
				enableDebug = True, targets = None,
				configureFlags = '',
				configureFunction = None):
	
	###################################
	
	_addMethodUnpack(env)
	_addMethodConfigure(env)
	addMethodMake(env)

	if not unpackDir:
		unpackDir = Dir(defaultUnpackDir)
	if not statusDir:
		statusDir = Dir(defaultStatusDir)

	def _buildFunction(compileParams):

		localConfigureFlags = str(configureFlags)
		if configureFunction:

			params = ConfigureParams(
								env = compileParams.clonedEnv,
								platform = compileParams.platform,
								abi = compileParams.abi,
								build = compileParams.build
								)

			additionFlags = configureFunction(configureParams = params)

			if type(additionFlags) == str:
				localConfigureFlags += additionFlags

		realTarget = configureMake(env = compileParams.clonedEnv,
						unpackEnv = compileParams.originalEnv,
						projectName = projectName,
						tarball = tarball,
						unpackDir = unpackDir,
						statusDir = statusDir,
						targets = targets,
						configureFlags = localConfigureFlags,
						compileParams = compileParams)

		return realTarget

	###################################

	targets = Targets.buildCrossCompileTargets(env = env,
						projectName = projectName,
						buildFunction = _buildFunction)

	distcleanTargets = [ unpackDir, statusDir, env.getVPATH() ]
	# Is out dir under project dir.
	if env.getPREFIX().abspath.startswith(Dir('#').abspath):
		distcleanTargets.append(env.getPREFIX())
	env.addForDistclean(distcleanTargets)

	return targets
	
	###################################


