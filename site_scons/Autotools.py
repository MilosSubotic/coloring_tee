'''
@file: Autotools.py
@date: Okt 17, 2012

@brief: Support for SCons to unpack, configure and make tarballs.
@version: 4.0 

@author: Milos Subotic milos.subotic.sm@gmail.com
@license: MIT
'''

###############################################################################

from __future__ import print_function

import os
import sys
import shutil
import copy

from SCons.Script import *

from Utils import Status

import Make
import Tarball
import Dirs
import Targets
import Patch
import Utils

###############################################################################

def _Configure(env, dirs, prerequisites, configureFlags, shellFlags):

	configure = dirs.packageUnpackDir.File('configure')
	
	configureCmd = 'cd ' + dirs.packageBuildDir.abspath + '; ' 
	configureCmd += shellFlags + ' '
	configureCmd += configure.abspath + ' ' + configureFlags
	configureCmd += ' --prefix=' + dirs.packageOutDir.abspath
	
	actions = []
	# If unpack and build dirs are same do not delete build dir.
	if dirs.packageUnpackDir != dirs.packageBuildDir:
		actions += [
				Delete(dirs.packageBuildDir),
				Mkdir(dirs.packageBuildDir)
		]
				
	actions += [ 
			configureCmd,
			Status(dirs.configured)
	]
	
	return env.Command(target = dirs.configured,
			source = prerequisites,
			action = actions)
	
def addMethodConfigure(env):
	env.AddMethod(_Configure, "Configure")

###############################################################################
	
class Autotools(Make.Make):
	
	def clone(self):
		return copy.copy(self)
		
	def createConfigureFlags(self, env, configureFlags, buildParams):
		# TODO Add for others.
		if buildParams.platform == 'linux' and buildParams.abi == 'i686':
			configureFlags += ' --host=i686-linux-gnu'
		return configureFlags
			
	def configure(self, env, dirs, configureFlags, shellFlags, 
				buildParams, prerequisites):
		return env.Configure(
				dirs = dirs,
				prerequisites = prerequisites,
				configureFlags = configureFlags,
				shellFlags = shellFlags)
				
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
					
		Make.Make.__init__(self, 
				buildParams,
				tarball,
				dirInTarball,
				patchFiles,
				configureFlags,
				makeFlags,
				shellFlags,
				targets,
				prerequisites)
		
		for e in [buildParams.env, buildParams.sharedEnv]:
			addMethodConfigure(e)

###############################################################################
		
class AutotoolsWithCrossCompileFlagsSet(Autotools):

	def createConfigureFlags(self, env, configureFlags, buildParams):
		configureFlags += Autotools.createConfigureFlags(self, env, \
				configureFlags, buildParams)

		configureFlags += ' --enable-static --disable-shared --with-pic=yes'
					
		if buildParams.platform == 'android':

			cppflags = Utils.listToString(' ', env['CPPFLAGS'])
			cppflags += Utils.listToString(' -D', env['CPPDEFINES'])
			cppflags += Utils.listToString(' -I', env['CPPPATH'])
			ldflags = Utils.listToString(' ', env['LINKFLAGS'])
			ldflags += Utils.listToString(' -L', env['LIBPATH'])			
			flags = ' --host={ARCH}-linux CC="{CC}" CXX="{CXX}" ' \
'CPP="{CPP}" CXXCPP="{CXXCPP}" CCFLAGS="{CCFLAGS}" CXXFLAGS="{CXXFLAGS}" ' \
'CCFLAGS="{CCFLAGS}" CPPFLAGS="{CPPFLAGS}" LDFLAGS="{LDFLAGS}" ' \
'LIBS="{LIBS}" PKG_CONFIG="PKG_CONFIG_LIBDIR=\'\' pkg-config" '.format(
					ARCH = env['ARCH'],
					CC = env['CC'],
					CXX = env['CXX'],
					CPP = env['CC'] + ' -E',
					CXXCPP = env['CXX'] + ' -E',
					CFLAGS = Utils.listToString(' ', env['CFLAGS']),
					CXXFLAGS = Utils.listToString(' ', env['CXXFLAGS']),
					CCFLAGS = Utils.listToString(' ', env['CCFLAGS']),
					CPPFLAGS = cppflags,
					LDFLAGS = ldflags,
					LIBS = Utils.listToString(' -l', env['LIBS'])
			)
					
			return configureFlags + flags
		else:
			return configureFlags

###############################################################################

