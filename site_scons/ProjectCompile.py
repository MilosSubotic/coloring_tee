'''
@file: ProjectCompile.py
@date: Okt 19, 2012

@brief: Support to set up SCons.
@version: 2.0

@author: Milos Subotic milos.subotic.sm@gmail.com
@license: MIT
'''
###############################################################################

from __future__ import print_function

from SCons.Script import *

import shutil

import Utils
import Targets

###############################################################################

def _rmDirIfExist(*args):
	dir = os.path.join(*args)
 	if os.path.exists(dir):
 		print('Remove directory: ' + dir)
 		shutil.rmtree(dir)

###############################################################################

class ProjectCompileParams(Targets.CompileParams):
	def __init__(self, compileParams, portBuildDir, projectName):

		Targets.CompileParams.__init__(self, compileParams.clonedEnv, 
				compileParams.originalEnv, compileParams.platform, 
				compileParams.abi, compileParams.build)
		self.portBuildDir = portBuildDir
				
		self.projectName = projectName



###############################################################################

def crossCompile(env, projectName, buildFunction, VPATH, targetFilter = None):
	'''
		@param VPATH: type Dir
	'''

	def _buildFunction(compileParams):

		portBuildDir = VPATH.Dir(compileParams.platformAbiBuild).Dir( 
				Utils.rootProjectRelativePath())
		
		compileParams.clonedEnv.VariantDir(portBuildDir, ".", duplicate=0)

		params = ProjectCompileParams(compileParams = compileParams, 
				portBuildDir = portBuildDir,
				projectName = projectName)

		return buildFunction(projectCompileParams = params)

	###################################

	targets = Targets.buildCrossCompileTargets(env = env,
						projectName = projectName,
						buildFunction = _buildFunction,
						targetFilter = targetFilter)
	
	return targets

###############################################################################


def staticLibraryBuildFunction(projectCompileParams):

	src = Utils.glob('*.c', '.') + Utils.glob('*.cpp', '.')

	buildDir = projectCompileParams.portBuildDir
	src = [ buildDir.File(s) for s in src ]
	target = buildDir.File(staticLibraryName(projectCompileParams.clonedEnv, 
			projectCompileParams.projectName))

	return projectCompileParams.clonedEnv.StaticLibrary(target, src)


def staticLibraryBuildFunctionRecursive(projectCompileParams):

	src = Utils.recursiveGlob('*.c', '.') + Utils.recursiveGlob('*.cpp', '.')

	buildDir = projectCompileParams.portBuildDir
	src = [ buildDir.File(s) for s in src ]
	target = buildDir.File(staticLibraryName(projectCompileParams.clonedEnv, 
			projectCompileParams.projectName))

	return projectCompileParams.clonedEnv.StaticLibrary(target, src)


def programBuildFunction(projectCompileParams):

	src = Utils.glob('*.c', '.') + Utils.glob('*.cpp', '.')

	buildDir = projectCompileParams.portBuildDir
	src = [ buildDir.File(s) for s in src ]
	target = buildDir.File(programName(projectCompileParams.clonedEnv, 
			projectCompileParams.projectName))

	return projectCompileParams.clonedEnv.Program(target, src)




def programBuildFunctionRecursive(projectCompileParams):

	src = Utils.recursiveGlob('*.c', '.') + Utils.recursiveGlob('*.cpp', '.')

	buildDir = projectCompileParams.portBuildDir
	src = [ buildDir.File(s) for s in src ]
	target = buildDir.File(programName(projectCompileParams.clonedEnv, 
			projectCompileParams.projectName))

	return projectCompileParams.clonedEnv.Program(target, src)


	
###############################################################################

def sharedLibraryName(env, name):
	# TODO This is original but it not working
	# return env['SHLIBPREFIX'] + name + env['SHLIBSUFFIX']
	return env['LIBPREFIX'] + name + env['SHLIBSUFFIX']

def staticLibraryName(env, name):
	return env['LIBPREFIX'] + name + env['LIBSUFFIX']

def programName(env, name):
	return env['PROGPREFIX'] + name + env['PROGSUFFIX']


###############################################################################



