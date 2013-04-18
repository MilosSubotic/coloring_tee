'''
@file: Utils.py
@date: Okt 19, 2012

@brief: Support to set up SCons.
@version: 2.0 

@author: Milos Subotic milos.subotic.sm@gmail.com
@license: MIT
'''
###############################################################################

from __future__ import print_function

from SCons.Script import *

import os
import fnmatch
from glob import glob as _originalGlob
import datetime

###############################################################################

def printWarning(message):
	Main._scons_user_warning(message)

###############################################################################

def rootProjectRelativePath():
	'''
	@return: Relative path of current directory from project root directory.
	'''
	return Dir('#').rel_path(Dir('.'))

def buildDir(vpath):
	return vpath.Dir(rootProjectRelativePath())

###############################################################################
	
class GlobalConfig:

	def cxx0xCXXFLAGS(self): 
		return [ '-std=c++0x', '-D__STDC_INT64__' ]
		
	def __init__(self):
		self.targetFilter = None

###############################################################################
	
def addPathVariable(env, name, help, default, 
		validator = PathVariable.PathIsDir):
	variable = Variables(None, ARGUMENTS)

	variable.Add(PathVariable(name, help, default, validator))
	variable.Update(env)
	
	env.Help(variable.GenerateHelpText(env))


def constructSConsVariables():
	variables = Variables(None, ARGUMENTS)

	variables.Add(PathVariable('VPATH', 'Path to build directory',
						'build', PathVariable.PathIsDirCreate))

	variables.Add(PathVariable('PREFIX', 'Path to install directory',
						'/usr/local', PathVariable.PathIsDirCreate))

	existingAndroidNDKPath = None
	try:
		existingAndroidNDKPath = os.environ['ANDROID_NDK']
	except KeyError:
		pass
	variables.Add(PathVariable('ANDROID_NDK', 'Path to Android NDK directory',
						existingAndroidNDKPath, PathVariable.PathIsDir))

	return variables



def constructSConsEnvironment():
	variables = constructSConsVariables()
	env = Environment(variables = variables)

	env.Help(variables.GenerateHelpText(env))

	#env.Decider('MD5-timestamp')
	env.Decider('timestamp-newer')

	envVars = [ 'CPPFLAGS', 'CPPPATH', 'CFLAGS', 'CXXFLAGS', 
				'LINKFLAGS', 'LIBS', 'LIBPATH' ]
	for var in envVars:
		try:
			env[var]
		except KeyError:
			env[var] = SCons.Util.CLVar()


	env.globalConfig = GlobalConfig()

	# For enabling linking static to shared libraries.
	env['STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME'] = 1

	# Methods for geting VPATH.
	def getVPATH(env):
		return Dir('#').Dir(env['VPATH'])
	env.AddMethod(getVPATH, 'getVPATH')

	def getPREFIX(env):
		return Dir('#').Dir(env['PREFIX'])
	env.AddMethod(getPREFIX, 'getPREFIX')

	# Method for checking is project root project.
	def isRootProject(env):
		return rootProjectRelativePath() == '.'
	env.AddMethod(isRootProject, 'isRootProject')

	env['DISTCLEAN_TARGETS'] = recursiveGlob('*.pyc', Dir('#').abspath)
	env['DISTCLEAN_TARGETS'].append(env.getVPATH())
	def addForDistclean(env, target):
		if type(target) == list:
			env['DISTCLEAN_TARGETS'] += target
		else:
			env['DISTCLEAN_TARGETS'].append(target)
	env.AddMethod(addForDistclean, 'addForDistclean')

	return env


def cloneSConsEnvironment(env):

	envVars = [ 'CPPFLAGS', 'CPPPATH', 'CFLAGS', 'CXXFLAGS', 
				'LINKFLAGS', 'LIBS', 'LIBPATH' ]

	clone = env.Clone()
	for var in envVars:
		try:
			clone[var] = SCons.Util.CLVar()
			clone[var] += env[var]
		except KeyError:
			pass
	
	# Preserve global distclean targets.
	clone['DISTCLEAN_TARGETS'] = env['DISTCLEAN_TARGETS']
	
	return clone

	
def addDistcleanTargets(env):
	env.Clean('distclean', env['DISTCLEAN_TARGETS'])
	env.Help('\nClean targets (used with -c flag):\n\ndistclean\n')
	
def addTarballTarget(env, projectName):
	now = datetime.datetime.now()
	dateTime = "%d-%02d-%02d-%02d-%02d-%02d" % (now.year, now.month, now.day,
			now.hour, now.minute, now.second) 
	
	cmd = "cd ..; tar cfvj " + projectName + \
		"-" + dateTime + ".tar.bz2 " + projectName
	
	tarballCommand = env.Command('tarball', None, [ 
			Delete(env['DISTCLEAN_TARGETS']), cmd ])
	
	env.Help('\nUtils targets:\n\ntarball\n')

###############################################################################

def recursiveGlob(pattern, directory = '.'):
	found = []
	for root, dirs, files in os.walk(directory):
		for basename in files:
			if fnmatch.fnmatch(basename, pattern):
				filename = os.path.join(root, basename)
				found.append(filename)
	return found

def glob(pattern, directory = '.'):
	return _originalGlob(os.path.join(directory, pattern))

###############################################################################

class ConfigFile:

	def __init__(self, configFileName):
		code = ''
		codeGlobals = {}
		codeLocals = {}

		fileName = File(configFileName)
		if not fileName.exists():
			raise SCons.Errors.StopError(
					"Config file '{0}' not exists!".format(fileName.abspath))			
			
		with open(fileName.abspath) as f:
			code = f.read()
		
		exec code in codeGlobals, codeLocals
	
		self._codeLocals = codeLocals
		self._configFileName = configFileName

	def getDirPath(self, name):
		'''
		Index operator
		'''
		try:
			pathOrPaths = self._codeLocals[name]
		except KeyError:
			printWarning("'{0}' not set in config file '{1}'".format(
				name, self._configFileName))
			return ''
		
		if type(pathOrPaths) != list:
			paths = [ pathOrPaths ]
		else:
			paths = pathOrPaths

		for path in paths:
			if Dir(path).exists():
				return path

		printWarning("Any of '{0}' paths defined in '{1}' do not exist".format(
				name, self._configFileName))
		return ''

###############################################################################
	
def isAndroidProjectDebuggable(pathToAndroidManifest = 'AndroidManifest.xml'):
	'''
		Check in AndroidManifest.xml if Android project is debuggable.
		@param pathToAndroidManifest: Path to Android manifest file.
	'''
	from xml.dom import minidom

	xmlDoc = minidom.parse(pathToAndroidManifest)

	# Assume there is only one application tag.
	applicationTag = xmlDoc.getElementsByTagName('application')[0]

	try:
		s = applicationTag.attributes['android:debuggable'].value
		if s == 'true':
			debuggable = True
		else:
			debuggable = False
	except KeyError:
		debuggable = False

	return debuggable


###############################################################################


