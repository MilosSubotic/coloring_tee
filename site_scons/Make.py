'''
@file: Make.py
@date: Okt 17, 2012

@brief: Support for SCons to make.
@version: 3.0 

@author: Milos Subotic milos.subotic.sm@gmail.com
@license: MIT
'''

###############################################################################

from __future__ import print_function

###############################################################################

__date__ = '2012/10/17 00:00:00'

__version__ = '3.0'
__license__ = 'MIT'

__author__ = 'Milos Subotic milos.subotic.sm@gmail.com'
__status__ = 'Beta'

###############################################################################

import os

from SCons.Script import *

from ShellHelpers import *

from Utils import Status

import Tarball
import Patch
import Dirs

###############################################################################


def make(env, targets, packageDir):
	if type(targets) == str:
		targets = [targets]

	jobs = '-j' + str(env.GetOption('num_jobs'))

	startDir = os.getcwd() # Restore PWD at the end.
	os.chdir(packageDir) # cd to package directory.

	if env.GetOption('clean'):
		run('make clean ' + jobs)
	else:
		for target in targets:
			run('make V=1' + target + ' ' + jobs)

	os.chdir(startDir) # Restore the original PWD.


class MakeWarning(SCons.Warnings.Warning):
	pass

class MakeNotFound(MakeWarning):
	pass

SCons.Warnings.enableWarningClass(MakeWarning)

def _detectMake(env):
	''' Try to detect the "make"'''
	try:
		return env['MAKE']
	except KeyError:
		pass

	tar = env.WhereIs('make')
	if tar:
		return tar

	raise SCons.Errors.StopError(
		MakeNotFound,
		'Could not detect "make"')
	return None

def _Make(env, statusFile, makeTarget, prerequisites, buildDir, 
		targetFile = None, flags = '', shellFlags = '', makeFile = None):
	jobs = env.GetOption('num_jobs')
	
	if makeFile:
		makeFileFlag = '-f "{0}"'.format(makeFile.abspath)
	else:
		makeFileFlag = ''

	makeAction = Action(
			'{shellFlags} $MAKE {makeFileFlag} -C "{buildDir}" -j{jobs} \
{flags} {target}'.format(
					shellFlags = shellFlags,
					makeFileFlag = makeFileFlag,
					buildDir = buildDir.abspath, 
					jobs = str(jobs), 
					flags = flags,
					target = makeTarget
					)
			)

	if statusFile:
		return env.Command(target = statusFile,
				source = prerequisites,
				action = [ makeAction, 
						Status(statusFile) ]
				)
	else:
		return env.Command(target = targetFile,
				source = prerequisites,
				action = [ makeAction ]
				)

def addMethodMake(env):
	env['MAKE'] = _detectMake(env)
	env.AddMethod(_Make, "Make")
	
	
###############################################################################
	
class Make(object):
	
	def clone(self):
		return copy.copy(self)
		
	def unpack(self, env, sharedEnv, dirs, tarball, dirInTarball, 
				withVariantDir, prerequisites):
		if withVariantDir:
			unpackEnv = sharedEnv
		else:
			unpackEnv = env
		return unpackEnv.Unpack(dirs = dirs,
				tarball = tarball,
				dirInTarball = dirInTarball,
				prerequisites = prerequisites)

	def patch(self, env, sharedEnv, dirs, withVariantDir, prerequisites):
		if not self.patchFiles:
			return prerequisites	
		else:
			if withVariantDir:
				patchEnv = sharedEnv
			else:
				patchEnv = env
				
			target = prerequisites
			for patchFile in self.patchFiles:
				mangledPatchFile = str(patchFile).replace('/', '~')
				statusFile = dirs.variantDirDependedStatusDir.File(
						'patched_with~{0}'.format(mangledPatchFile))
				target = patchEnv.Patch(statusFile = statusFile, 
						patchDir = dirs.packageUnpackDir, 
						patchFile = patchFile,
						patchLevel = 1, 
						prerequisites = target)
			return target
			
	def createConfigureFlags(self, env, configureFlags, buildParams):
		return configureFlags
			
	def createShellFlags(self, env, shellFlags, buildParams):
		return shellFlags
			
	def configure(self, env, dirs, configureFlags, shellFlags, 
				buildParams, prerequisites):
		return prerequisites
		
	def createMakeFlags(self, env, dirs, buildParams, flags = ''):
		if env['VERBOSE']:
			flags += 'V=1 '
		else:
			flags += 'V="" '
		return flags
		
	def makeAll(self, env, dirs, flags, shellFlags, prerequisites):
		return env.Make(statusFile = dirs.compiled, 
				makeTarget = 'all', 
				prerequisites = prerequisites,
				buildDir = dirs.packageBuildDir,
				flags = flags,
				shellFlags = shellFlags)
				
	def makeInstall(self, env, dirs, flags, shellFlags, prerequisites):
		return env.Make(statusFile = dirs.installed, 
				makeTarget = 'install', 
				prerequisites = prerequisites,
				buildDir = dirs.packageBuildDir,
				flags = flags,
				shellFlags = shellFlags)
				
	def make(self, env, dirs, buildParams, makeFlags, 
				shellFlags, prerequisites):
		compiled = self.makeAll(env = env,
				dirs = dirs,
				flags = makeFlags,
				shellFlags = shellFlags,
				prerequisites = prerequisites)
	
		return self.makeInstall(env = env,
				dirs = dirs,
				flags = makeFlags,
				shellFlags = shellFlags,
				prerequisites = compiled)
				
	def makeTargets(self, env, dirs, buildParams, makeFlags, 
			shellFlags,	targets, prerequisites):
		nextTarget = prerequisites
		for target in targets:
			nextTarget = env.Make(
					statusFile = dirs.buildStatusDir.File(target),
					makeTarget = target, 
					buildDir = dirs.packageBuildDir,
					flags = makeFlags,
					shellFlags = shellFlags,
					prerequisites = nextTarget)	
		return nextTarget
				
	def __init__(self, 
			buildParams,
			tarball,
			dirInTarball = None,
			patchFiles = [],
			configureFlags = '',
			makeFlags = '',
			shellFlags = '',
			targets = None,
			prerequisites = None):
			
		###################################
		
		self.buildParams = buildParams
		self.tarball = tarball
		self.dirInTarball = dirInTarball
		self.prerequisites = prerequisites
		self.patchFiles = patchFiles
		self.configureFlags = configureFlags
		self.shellFlags = shellFlags
		self.makeFlags = makeFlags
		self.targets = targets
		dirs = buildParams.dirs
		
		###################################
		# Add necessary methods.
		
		for e in [buildParams.env, buildParams.sharedEnv]:
			Tarball.addMethodUnpack(e)
			addMethodMake(e)
			Patch.addMethodPatch(e)
		
		###############################
		# Add for distclean.
		'''
		self.buildParams.env.addForDistclean([
			dirs.statusDir,
			dirs.packageUnpackDir,
			dirs.packageBuildDir,
			dirs.packageOutDir
		])	
		'''
		
	def run(self):
	
		###############################
		# Unpack.

		unpacked = self.unpack(env = self.buildParams.env, 
				sharedEnv = self.buildParams.sharedEnv, 
				dirs = self.buildParams.dirs,
				tarball = self.tarball,
				dirInTarball = self.dirInTarball,
				withVariantDir = self.buildParams.withVariantDir,
				prerequisites = None)
				
		###############################
		# Patch.
		
		patched = self.patch(env = self.buildParams.env, 
				sharedEnv = self.buildParams.sharedEnv, 
				dirs = self.buildParams.dirs, 
				withVariantDir = self.buildParams.withVariantDir,
				prerequisites = unpacked)
		
		if self.prerequisites:
			patched += self.prerequisites

		###############################
		# Configure.

		self.configureFlags = self.createConfigureFlags(env = self.buildParams.env,
				configureFlags = self.configureFlags,
				buildParams = self.buildParams)
		self.shellFlags = self.createShellFlags(env = self.buildParams.env, 
				shellFlags = self.shellFlags, 
				buildParams = self.buildParams)

		configured = self.configure(
				env = self.buildParams.env,
				dirs = self.buildParams.dirs,
				prerequisites = patched,
				configureFlags = self.configureFlags,
				shellFlags = self.shellFlags,
				buildParams = self.buildParams)

		###############################
		# Make targets.
		
		self.makeFlags = self.createMakeFlags(env = self.buildParams.env, 
				dirs = self.buildParams.dirs,
				buildParams = self.buildParams,
				flags = self.makeFlags)
				
		# If targets are not specified do standard all and install targets.
		if not self.targets:
			
			return self.make(env = self.buildParams.env, 
					dirs = self.buildParams.dirs, 
					buildParams = self.buildParams,
					makeFlags = self.makeFlags,
					shellFlags = self.shellFlags,
					prerequisites = configured)
		
		else:
			if not SCons.Util.is_List(self.targets):
				self.targets = [self.targets]
					
			return self.makeTargets(env = self.buildParams.env, 
					dirs = self.buildParams.dirs, 
					buildParams = self.buildParams,
					makeFlags = self.makeFlags,
					shellFlags = self.shellFlags,
					targets = self.targets,
					prerequisites = configured)

###############################################################################	
	
