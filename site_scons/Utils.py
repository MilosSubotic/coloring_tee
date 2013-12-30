'''
@file: Utils.py
@date: Okt 19, 2012

@brief: Support to set up SCons.
@version: 3.0 

@author: Milos Subotic milos.subotic.sm@gmail.com
@license: MIT
'''
###############################################################################

from __future__ import print_function

from SCons.Script import *
import SCons

import os
import fnmatch
from glob import glob as _originalGlob
import datetime
import uuid
import zipfile
import tarfile
import platform

###############################################################################

def printWarning(message):
	Main._scons_user_warning(message)

def getTimeStamp():
	now = datetime.datetime.now()
	timeStamp = "%d-%02d-%02d-%02d-%02d-%02d" % (now.year, now.month, now.day,
			now.hour, now.minute, now.second) 
	return timeStamp

def getCurrentDirName():
	return os.path.basename(os.getcwd())

###############################################################################

class NotSupportedPlatformWarning(SCons.Warnings.Warning):
	pass

SCons.Warnings.enableWarningClass(NotSupportedPlatformWarning)

def hostPlatform():
	if platform.system() == 'Linux':
		return 'linux'
	else:
		raise SCons.Errors.StopError(
			NotSupportedPlatformWarning, 
			'Not supported host platform "{0}" for now!'.format(
				platform.system()
			)
		)

def hostABI():
	if hostPlatform() == 'linux':
		if platform.machine() == 'x86_64':
			return 'x86_64'
		else:
			return 'i686'

###############################################################################

def rootProjectRelativePath():
	'''
	@return: Relative path of current directory from project root directory.
	'''
	return Dir('#').rel_path(Dir('.'))

def isUnderProjectDir(node):
	if isinstance(node, SCons.Node.FS.Base):
		return node.abspath.startswith(Dir('#').abspath)
	elif isinstance(node, str):
		return dep.startswith(Dir('#').abspath)
	else:
		raise SCons.Errors.StopError('Parameter for isUnderProjectDir() '\
				'must be of type: File, Dir, or str')

###############################################################################

def recursiveGlob(pattern, directories = Dir('.')):
	if SCons.Util.is_List(directories):
		directories = map(Dir, directories)
	else:
		directories = [ Dir(directories) ]

	found = []
	for directory in directories:
		for root, dirs, files in os.walk(str(directory), followlinks = True):
			root = Dir(root)
			for baseName in files:
				if fnmatch.fnmatch(baseName, pattern):
					found.append(root.File(baseName))
			for baseName in dirs:
				if fnmatch.fnmatch(baseName, pattern):
					found.append(root.Dir(baseName))			
	return found

def glob(pattern, directories = Dir('.')):
	if SCons.Util.is_List(directories):
		directories = map(Dir, directories)
	else:
		directories = [ Dir(directories) ]

	found = []
	for directory in directories:
		filesOrDirs = _originalGlob(os.path.join(str(directory), pattern))
		for fileOrDir in filesOrDirs:
			if os.path.isdir(fileOrDir):
				found.append(Dir(fileOrDir))
			else:
				found.append(File(fileOrDir))
	return found

###############################################################################

def listToString(prepend, listForJoin):
	return prepend.join([''] + listForJoin)

###############################################################################
# Actions.

def _symlink_func(dest, src, force=False):
	if not SCons.Util.is_List(dest):
		dest = [dest]
	for entry in dest:
		try:
			os.symlink(str(src), str(entry))
		except os.error, e:
			if force:
				pass
			else:
				raise

def _symlink_strfunc(dest, src, force=False):
	return 'Symlink("%s", "%s")' % (SCons.Defaults.get_paths_str(dest), 
			str(src))

Symlink = SCons.Action.ActionFactory(_symlink_func, _symlink_strfunc)

#######################################

def _status_func(status):
	if not SCons.Util.is_List(status):
		status = [status]
	for s in status:
		s = str(s)
		SCons.Defaults.mkdir_func(os.path.dirname(s))
		SCons.Node.FS.invalidate_node_memos(s)
		with open(s, 'w') as f:
			f.write('Date and time: ')
			f.write(getTimeStamp())
			f.write('\nUUID: ')
			f.write(str(uuid.uuid4()))
			f.write('\nStatus: ')
			f.write(os.path.basename(s))
			f.write('\n')
			f.close()
	
def _status_strfunc(status):
	return 'Status("%s")' % (SCons.Defaults.get_paths_str(status))

Status = SCons.Action.ActionFactory(_status_func, _status_strfunc)

#######################################

def _zip_func(dest, src, chdir='.', ignore=[]):
	if not SCons.Util.is_List(src):
		src = [src]
	if not SCons.Util.is_List(ignore):
		ignore = [ignore]

	ignore = [ os.path.abspath(str(i)) for i in ignore ]
	chdir = str(chdir)
	chdirLen = len(chdir)
	dest = str(dest)
	destFileName = os.path.join(chdir, dest)

	try:
		destFile = zipfile.ZipFile(destFileName, 'w', zipfile.ZIP_DEFLATED)
		for s in src:
			s = os.path.join(chdir, str(s))
			for dirpath, dirs, files in os.walk(s):
				absDirpath = os.path.abspath(dirpath)
				if not any([ absDirpath.startswith(i) for i in ignore]):
					for f in files:
						fileName = os.path.join(dirpath, f)
						if not os.path.abspath(fileName) in ignore:
							if fileName.startswith(chdir):
								archName = fileName[chdirLen:]
							else:
								archName = fileName
							destFile.write(fileName, archName)
	finally:
		destFile.close()

def _zip_strfunc(dest, src, chdir='.', ignore=[]):
	return 'Zip("%s", "%s", "%s")' % (dest, SCons.Defaults.get_paths_str(src), 
			chdir)

Zip = SCons.Action.ActionFactory(_zip_func, _zip_strfunc)

#######################################

def _tar_bz2_func(dest, src, chdir='.', ignore=[]):
	if not SCons.Util.is_List(src):
		src = [src]
	if not SCons.Util.is_List(ignore):
		ignore = [ignore]

	ignore = [ os.path.abspath(str(i)) for i in ignore ]
	chdir = str(chdir)
	chdirLen = len(chdir)
	dest = str(dest)
	destFileName = os.path.join(chdir, dest)

	try:
		destFile = tarfile.open(destFileName, 'w:bz2')
		for s in src:
			s = os.path.join(chdir, str(s))
			for dirpath, dirs, files in os.walk(s):
				absDirpath = os.path.abspath(dirpath)
				if not any([ absDirpath.startswith(i) for i in ignore]):
					for f in files:
						fileName = os.path.join(dirpath, f)
						if not os.path.abspath(fileName) in ignore:
							if fileName.startswith(chdir):
								archName = fileName[chdirLen:]
							else:
								archName = fileName
							destFile.add(fileName, archName)
	finally:
		destFile.close()	

def _tar_bz2_strfunc(dest, src, chdir='.', ignore=[]):
	return 'TarBz2("%s", "%s", "%s")' % (dest, 
			SCons.Defaults.get_paths_str(src), chdir)

TarBz2 = SCons.Action.ActionFactory(_tar_bz2_func, _tar_bz2_strfunc)

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
		
		if not SCons.Util.is_List(pathOrPaths):
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


