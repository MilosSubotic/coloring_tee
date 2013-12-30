'''
@file: Dirs.py
@date: Apr 20, 2013

@brief: Helpers for directories and build status. 
	Used primary by Autotools.py and CMake.py.
@version: 1.0 

@author: Milos Subotic milos.subotic.sm@gmail.com
@license: MIT
'''

###############################################################################

from __future__ import print_function

from SCons.Script import *

import copy

###############################################################################

class Dirs:

	def __init__(self, env, packageName, platformAbiBuild, withVariantDir):
		self.packageName = packageName
		self.buildDir = env.getVPATH()
		self.statusDir = self.buildDir.Dir('.status')
		self.unpackDir = self.buildDir.Dir('unpack')
		self.platformAbiBuild = platformAbiBuild

		# Status
		if withVariantDir:
			self.variantDirDependedStatusDir = self.statusDir.Dir(packageName)
		else:
			self.variantDirDependedStatusDir = self.statusDir.Dir(packageName
					).Dir(platformAbiBuild)
		self.buildStatusDir = self.statusDir.Dir(packageName).Dir(
				platformAbiBuild)
		self.unpacked = self.variantDirDependedStatusDir.File('unpacked')
		self.cmaked = self.buildStatusDir.File('cmaked')
		self.patched = self.buildStatusDir.File('patched')
		self.configured = self.buildStatusDir.File('configured')
		self.compiled = self.buildStatusDir.File('compiled')
		self.installed = self.buildStatusDir.File('installed')

		# Dirs
		self.packageBuildDir = self.buildDir.Dir(packageName).Dir(
				platformAbiBuild)
		self.packageOutDir = env.getPREFIX().Dir(packageName).Dir(
				platformAbiBuild)
		if withVariantDir:
			self.packageUnpackDir = self.unpackDir.Dir(packageName)
		else:
			self.packageUnpackDir = self.packageBuildDir
		
	def clone(self):
		return copy.copy(self)


