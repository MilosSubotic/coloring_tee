'''
@file: Make.py
@date: Apr 20, 2013

@brief: Support for SCons to make.
@version: 1.0 

@author: Milos Subotic milos.subotic.sm@gmail.com
@license: MIT
'''

__date__ = '2013/14/20 11:12:00'

__version__ = '1.0'
__license__ = 'MIT'

__author__ = 'Milos Subotic milos.subotic.sm@gmail.com'
__status__ = 'Beta'


###############################################################################

from SCons.Script import *

import Utils

###############################################################################

class QMakeWarning(SCons.Warnings.Warning):
	pass

class QMakeNotFound(QMakeWarning):
	pass

SCons.Warnings.enableWarningClass(QMakeWarning)

def _detectQMake(env):
	''' Try to detect the "qmake"'''
	try:
		return env['QMAKE']
	except KeyError:
		pass

	tar = env.WhereIs('qmake-qt4')
	if tar:
		return tar

	raise SCons.Errors.StopError(QMakeNotFound, 
			'Could not detect "qmake-qt4"')
	return None

def QMake(env, projectDir, prerequisite = None, projectFile = None, 
		options = ''):
	if prerequisite:
		if not SCons.Util.is_List(targets):
			prerequisite = [prerequisite]
	else:
		prerequisite = []
	
	if projectFile:
		prerequisite.append(projectFile)
	else:
		proFiles = Utils.glob('*.pro', projectDir.abspath)
		if len(proFiles) > 0:
			projectFile = projectDir.File(proFiles[0])
			prerequisite.append(projectFile)

	return [env.Command(target = projectDir.File('Makefile'),
			source = prerequisite,
			action = [
				Action('cd {0}; $QMAKE {1} {2}'.format(projectDir.abspath, 
						projectFile.abspath, options)),
			]
			)]	

def addMethodQMake(env):
	env['QMAKE'] = _detectQMake(env)
	env.AddMethod(QMake, "QMake")

	
