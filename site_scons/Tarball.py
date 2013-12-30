'''
@file: Tarball.py
@date: Apr 20, 2013

@brief: Helpers for unpacking tarballs. 
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

	raise SCons.Errors.StopError(TarNotFound, 'Could not detect "tar"')
	return None
	
def _Unpack(env, dirs, tarball, dirInTarball = None, prerequisites = None):
	
	actions = [ 
				Mkdir(dirs.unpackDir),
				Delete(dirs.packageUnpackDir),
				Action('$TAR xfv {0} -C {1}'.format(tarball.abspath, 
						dirs.unpackDir.abspath)),
			]

	
	if not dirInTarball:
		tarballBasename = os.path.basename(tarball.abspath)
		dirInTarball = re.search(r'(.*).tar(.*)', 
				tarballBasename).group(1)
	packageUnpackDir = os.path.basename(dirs.packageUnpackDir.abspath)

	# If dir from unpacked tarball is different from dirs.packageUnpackDir
	# rename it to dirs.packageUnpackDir.
	if dirInTarball != packageUnpackDir:
		# TODO Instead of Move use recursive copy.
		actions.append(Move(dirs.packageUnpackDir, 
				dirs.unpackDir.Dir(dirInTarball)))

	actions.append(Status(dirs.unpacked))	
	
	return env.Command(target = dirs.unpacked,
			source = prerequisites,
			action = actions)


def addMethodUnpack(env):
	env['TAR'] = _detectTar(env)

	env.AddMethod(_Unpack, "Unpack")

###############################################################################

