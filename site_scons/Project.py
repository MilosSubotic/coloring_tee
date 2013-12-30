'''
@file: Project.py
@date: Aug 14, 2013

@brief: Project builder.
@version: 2.0 

@author: Milos Subotic milos.subotic.sm@gmail.com
@license: MIT
'''

###############################################################################

from __future__ import print_function

###############################################################################

__date__ = '2013/8/14 08:08:00'

__version__ = '1.0'
__license__ = 'MIT'

__author__ = 'Milos Subotic milos.subotic.sm@gmail.com'
__status__ = 'Beta'

###############################################################################

from SCons.Script import *
import SCons

import Utils

###############################################################################


	

###############################################################################

class Project(object):
	def __init__(self, 
			projectName, 
			targetFilter = lambda buildParams: True,
			defaultVPATH = 'build',
			defaultPREFIX = 'out'):
		self.projectName = projectName
		self.targetFilter = targetFilter
		self.globalEnv = self._createEnvironment(
			defaultVPATH = defaultVPATH,
			defaultPREFIX = defaultPREFIX
		)
		self.buildPackagesCalled = False
		#self._returnPackageInterfaces = {}
		
		class SConscriptArgs:
			def __init__(self, project):
				self.project = project

		sconscriptArgs = SConscriptArgs(self)
		Export('sconscriptArgs')

	def buildPackages(self, dirToPackageScript):
		'''
		@return Package interfaces of public packages in dirToPackageScript.
		'''
		
		# If first buildPackages then generate help for variables.
		if not self.buildPackagesCalled:
			self.globalEnv.Help(
				self.globalEnv.variables.GenerateHelpText(self.globalEnv)
			)
			self.buildPackagesCalled = True


		if not isinstance(dirToPackageScript, SCons.Node.FS.Dir):
			dirToPackageScript = Dir(dirToPackageScript)
		# TODO Find out better error name.
		if not dirToPackageScript.exists():
			raise SCons.Errors.StopError('Directory "{0}" no exists!'.format(
					dirToPackageScript))
		sconscriptFile = dirToPackageScript.File('SConscript')
		if not sconscriptFile.exists():
			raise SCons.Errors.StopError(
					'There is no SConscript file in directory "{0}"!'.format(
					dirToPackageScript))

		self._returnPackageInterfaces = {}
		SConscript(sconscriptFile)
		tmp = self._returnPackageInterfaces
		self._returnPackageInterfaces = {}
		return tmp

	def finish(self):
		env = self.globalEnv
	
		# Add help first, but real target is last.	
		env.Help('\nClean targets (used with -c flag):\n\ndistclean\n')

		projectDir = Utils.getCurrentDirName()

		ignoredFiles = [ File('#/.sconsign.dblite') ]
		ignoredFiles += env.distcleanTargets
	
		env.Help('\nUtils targets:\n')
	
		###############################
	
		tarballFilename = '{projectName}-{timeStamp}.tar.bz2'.format(
				projectName = self.projectName,
				timeStamp = Utils.getTimeStamp()
		)
	
		tarballCommand = env.Command('tarball', None, [
				Utils.TarBz2(
					tarballFilename, 
					projectDir, 
					chdir = '../', 
					ignore = ignoredFiles
				)
		])

		env.Help('\ntarball\n')

		###############################

		env.addForDistclean(Dir('#/doxygen'))

		doxygenCommand = env.Command('doxygen', None, [ 'doxygen' ])

		env.Help('\ndoxygen\n')

		###############################

		sdistFilename = '{projectName}-{timeStamp}.zip'.format(
				projectName = self.projectName,
				timeStamp = Utils.getTimeStamp()
		)

		sdistCommand = env.Command('sdist', None, [
				Utils.Zip(
					sdistFilename, 
					projectDir, 
					chdir = '../', 
					ignore = ignoredFiles
				)
		])

		env.Help('\nsdist\n')

		###############################

		env.Clean('distclean', env.distcleanTargets)


	def _createEnvironment(self, defaultVPATH, defaultPREFIX):

		# Construct variables.
		variables = Variables(None, ARGUMENTS)

		variables.Add(
			PathVariable(
				'VPATH', 
				'Path to build directory',
				str(defaultVPATH),
				PathVariable.PathIsDirCreate
			)
		)

		variables.Add(
			PathVariable(
				'PREFIX',
				'Path to install directory',
				str(defaultPREFIX), 
				PathVariable.PathIsDirCreate
			)
		)

		variables.Add(
			BoolVariable(
				'VERBOSE', 
				'Verbose output', 
				False
			)
		)

		existingAndroidNDKPath = None
		try:
			existingAndroidNDKPath = os.environ['ANDROID_NDK']
		except KeyError:
			pass
		variables.Add(
			PathVariable(
				'ANDROID_NDK', 
				'Path to Android NDK directory',
				existingAndroidNDKPath, 
				PathVariable.PathAccept
			)
		)

		###############################

		env = Environment(variables = variables)
		env.variables = variables

		###############################

		# Enable coloring from CMake and clang.
		try:
			env['ENV']['TERM'] = os.environ['TERM']
		except KeyError:
			env['ENV']['TERM'] = ''

		#env.Decider('MD5-timestamp')
		#env.Decider('timestamp-newer')
		env.Decider('MD5')

		# TODO Add more variables.
		envVars = [ 'CPPFLAGS', 'CPPPATH', 'CPPDEFINES', 'CFLAGS', 'CXXFLAGS', 
					'LINKFLAGS', 'LIBS', 'LIBPATH', 'CROSS_PREFIX' ]
		for var in envVars:
			try:
				env[var]
			except KeyError:
				env[var] = SCons.Util.CLVar()

		###############################

		# For enabling linking static to shared libraries.
		env['STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME'] = 1
	
		###############################

		# Methods for geting VPATH.
		def getVPATH(env):
			return Dir(env['VPATH'])
		env.AddMethod(getVPATH, 'getVPATH')

		def getPREFIX(env):
			return Dir(env['PREFIX'])
		env.AddMethod(getPREFIX, 'getPREFIX')

		def addVariable(env, variable):
			env.variables.Add(variable)
			env.variables.Update(env)
		env.AddMethod(addVariable, 'addVariable')

		# TODO Better.
		# Method for checking is project root project.
		def isRootPackage(env):
			return Utils.rootProjectRelativePath() == '.'
		env.AddMethod(isRootPackage, 'isRootPackage')

		###############################

		# TODO fill this list from .gitignore.
		env.distcleanTargets = Utils.recursiveGlob('*.pyc', Dir('#'))
		env.distcleanTargets += Utils.recursiveGlob('__pycache__', Dir('#'))
		env.distcleanTargets.append(env.getVPATH())

		def addForDistclean(env, target):
			if SCons.Util.is_List(target):
				env.distcleanTargets += target
			else:
				env.distcleanTargets.append(target)
		env.AddMethod(addForDistclean, 'addForDistclean')

		###############################

		def clone(env):
			# TODO Add all variables.
			envVars = [ 'CPPFLAGS', 'CPPPATH', 'CFLAGS', 'CXXFLAGS', 
						'LINKFLAGS', 'LIBS', 'LIBPATH' ]

			clone = env.Clone()
			for var in envVars:
				try:
					clone[var] = SCons.Util.CLVar()
					clone[var] += env[var]
				except KeyError:
					pass
	
			return clone

		env.AddMethod(clone, 'clone')

		return env

###############################################################################

