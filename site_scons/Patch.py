'''
@file: Patch.py
@date: Mat 13, 2013

@brief: Helpers for patch. 
	Used primary by Autotools.py and CMake.py.
@version: 1.0 

@author: Milos Subotic milos.subotic.sm@gmail.com
@license: MIT
'''

###############################################################################

from __future__ import print_function

from SCons.Script import *

from Utils import Status

import os
import re

###############################################################################

class PatchWarning(SCons.Warnings.Warning):
	pass

class PatchNotFound(PatchWarning):
	pass

SCons.Warnings.enableWarningClass(PatchWarning)

def _detectPatch(env):
	''' Try to detect the "patch"'''
	try:
		return env['PATCH']
	except KeyError:
		pass

	tar = env.WhereIs('patch')
	if tar:
		return tar

	raise SCons.Errors.StopError(PatchNotFound, 'Could not detect "patch"')
	return None
	
def _Patch(env, statusFile, patchDir, patchFile, patchLevel = 1, 
		prerequisites = None):

	cmd = 'cd {patchDir} && $PATCH -p{patchLevel} < {patchFile}'.format(
			patchDir = patchDir.abspath,
			patchFile = patchFile.abspath,
			patchLevel = patchLevel)
	
	return env.Command(target = statusFile,
			source = prerequisites,
			action = [ cmd, Status(statusFile) ]
			)



def addMethodPatch(env):
	env['PATCH'] = _detectPatch(env)

	env.AddMethod(_Patch, "Patch")

###############################################################################

