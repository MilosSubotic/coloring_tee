'''
@file: Make.py
@date: Okt 17, 2012

@brief: Support for SCons to make.
@version: 1.0 

@author: Milos Subotic milos.subotic.sm@gmail.com
@license: MIT
'''

__date__ = '2012/10/17 00:00:00'

__version__ = '1.0'
__license__ = 'MIT'

__author__ = 'Milos Subotic milos.subotic.sm@gmail.com'
__status__ = "Beta"


###############################################################################

import os

from SCons.Script import *

from ShellHelpers import *

###############################################################################


def make(env, targets, projectDir):
	if type(targets) == str:
		targets = [targets]

	jobs = '-j' + str(env.GetOption('num_jobs'))

	startDir = os.getcwd() # Restore PWD at the end.
	os.chdir(projectDir) # cd to project directory.

	if env.GetOption('clean'):
		run('make clean ' + jobs)
	else:
		for target in targets:
			run('make ' + target + ' ' + jobs)

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

def Make(env, statusFile, makeTarget, prerequisite, buildDir):
	jobs = '-j' + str(env.GetOption('num_jobs'))
	
	return [env.Command(target = statusFile,
			source = prerequisite,
			action = [
				Action('cd {0}; $MAKE {1} {2}'.format(
					buildDir.abspath, jobs, makeTarget)),
				Mkdir(statusFile)
			]
			)]	

def addMethodMake(env):
	env['MAKE'] = _detectMake(env)
	env.AddMethod(Make, "Make")

	
