'''
@file: Targets.py
@date: Okt 19, 2012

@brief: Support for SCons to make targets for cross compilation.
@version: 2.0 

@author: Milos Subotic milos.subotic.sm@gmail.com
@license: MIT
'''

###############################################################################

from __future__ import print_function

from SCons.Script import *

import os
import copy
import platform

import Utils

###############################################################################

builds = ['debug', 'release']
platforms = ['linux', 'android']
androidABIs = ['armeabi-v7a', 'armeabi', 'mips', 'x86']
linuxABIs = ['x86_64']

def _getABIs(platform):
	if platform == 'android':
		return androidABIs
	else:
		return linuxABIs

###############################################################################

class TargetTriple:
	def __init__(self, platform, abi, build):
		self.platform = platform
		self.abi = abi
		self.build = build

class CompileParams(TargetTriple):
	def __init__(self, clonedEnv, originalEnv, platform, abi, build):
		TargetTriple.__init__(self, platform, abi, build)
		self.clonedEnv = clonedEnv
		self.originalEnv = originalEnv
		self.platformAbiBuild = os.path.join(platform, abi, build)

###############################################################################


def androidToolchain(env, abi, platformLevel, toolchainVersion):
	'''
		@param abi: Possible: armeabi armeabi-v7a mips x86
		@param platformLevel: Possible: 3 4 5 8 9 14
		@param toolchainVersion: Possible: 4.4.3 4.6
	'''

	# Index with self.abi
	_archs = {
				'armeabi' : 'arm',
				'armeabi-v7a' : 'arm',
				'mips' : 'mips',
				'x86' : 'x86'
				}

	_toolchainNames = {
				'armeabi' : 'arm-linux-androideabi',
				'armeabi-v7a' : 'arm-linux-androideabi',
				'mips' : 'mipsel-linux-android',
				'x86' : 'x86'
				}

	_toolchainPrefixes = {
				'armeabi' : 'arm-linux-androideabi-',
				'armeabi-v7a' : 'arm-linux-androideabi-',
				'mips' : 'mipsel-linux-android-',
				'x86' : 'i686-linux-android-'
				}

	arch = _archs[abi]
	toolchainName = _toolchainNames[abi]
	toolchainPrefix = _toolchainPrefixes[abi]

	env['ARCH'] = arch
	#env['ABI'] = abi

	if not env['ANDROID_NDK']:
		raise RuntimeError('Path to Android NDK directory not defined!')

	ndk = env['ANDROID_NDK']


	env['TARGET_OS'] = 'linux'
	sysroot = os.path.join(ndk,
				'platforms/android-' + platformLevel + '/arch-' + arch)
	env['SYSROOT'] = sysroot

	toolchain = toolchainName + '-' + toolchainVersion

	toolchainDir = os.path.join(ndk, 'toolchains', toolchain, 'prebuilt')


	if platform.system() == 'Linux':
		if platform.machine() == 'x86_64' and \
				os.path.isdir(os.path.join(toolchainDir, 'linux-x86_64')):
			buildSystem = 'linux-x86_64'
		elif os.path.isdir(os.path.join(toolchainDir, 'linux-x86')):
			buildSystem = 'linux-x86'
		else:
			raise RuntimeError('There is no prebuild toolchain '\
				'for host machine in Android NDK!')
	else:
		raise NotImplementedError(
				'Host OS not supported by script for now!')


	crossPrefix = os.path.join(toolchainDir, buildSystem, 
		'bin', toolchainPrefix)
	env['CROSS_PREFIX'] = crossPrefix

	env['CPPPATH'].append(os.path.join(sysroot, 'usr/include'))
	env['CPPPATH'].append(os.path.join(ndk,
							'sources/cxx-stl/gnu-libstdc++/' +
							toolchainVersion + '/include'))
	env['CPPPATH'].append(os.path.join(ndk,
							'sources/cxx-stl/gnu-libstdc++/' +
							toolchainVersion + '/libs/' + abi + '/include'))


	env['CPPFLAGS'] += [ '-DANDROID' ]
	if abi == 'armeabi-v7a':
		env['CPPFLAGS'] += ['-march=armv7-a',
							 '-mfloat-abi=softfp', '-mfpu=neon']

	#env['CFLAGS']
	#env['CXXFLAGS']
	env['LIBS'] += ['supc++', 'gnustl_static', 'log']

	env['LIBPATH'].append(os.path.join(ndk,
									'sources/cxx-stl/gnu-libstdc++/' +
									toolchainVersion + '/libs/' + abi))
	env['LIBPATH'].append(os.path.join(sysroot, 'usr/lib'))

	env['LINKFLAGS'].append('--sysroot=' + sysroot)
	if abi == 'armeabi-v7a':
		env['LINKFLAGS'].append('-Wl,--fix-cortex-a8')
		

	env['CC'] = crossPrefix + 'gcc'
	env['CXX'] = crossPrefix + 'g++'
	env['LINK'] = crossPrefix + 'g++'


###############################################################################


def buildCrossCompileTargets(env, projectName, buildFunction, 
		targetFilter = None):
	'''
	@param targetFilter functor with TargetTriple as parameter and return
		true if triple is alowed. It override env.globalConfig.targetFilter.
	'''

	# If there is no local target filter set one from globalConfig.
	if not targetFilter:
		if env.globalConfig.targetFilter:
			targetFilter = env.globalConfig.targetFilter

	originalEnv = env
	aliases = []

	for platform in platforms:
		abis = _getABIs(platform)
		for abi in abis:
			for build in builds:
				if targetFilter:
					targetTriple = TargetTriple(platform, abi, build)
					if not targetFilter(targetTriple):
						continue

				clonedEnv = Utils.cloneSConsEnvironment(originalEnv)
				env['PLATFORM'] = platform
				env['ABI'] = abi
				env['BUILD'] = build

				if platform == 'android':
					# Hardcoded platformLevel and toolchainVersion.
					androidToolchain(env = clonedEnv,
								abi = abi,
								platformLevel = '14',
								toolchainVersion = '4.6')

				if build == 'release':
					clonedEnv['CPPFLAGS'].append('-O3')
				else:
					clonedEnv['CPPFLAGS'] += [ '-O0', '-g3' ]

				params = CompileParams(
								clonedEnv = clonedEnv,
								originalEnv = originalEnv,
								platform = platform,
								abi = abi,
								build = build
								)

				realTarget = buildFunction(compileParams = params)

				if realTarget:
					alias = projectName + '_' + platform + '_' + abi + '_' + \
								build
					aliases += env.Alias(alias, realTarget)


	s = '\nTargets for %s project:\n\n' % projectName
	for alias in aliases:
		s += '%s\n' % alias
	env.Help(s)

	return aliases



###############################################################################

def makeStandardTargets(env, sourceTargets):
	s = '\nStandard targets:\n\n'
	targets = []
	import re

	###############
	# platform_abi_build

	targetsGroup = []

	for platform in platforms:
		for abi in _getABIs(platform):
			for build in builds:
				alias = platform + '_' + abi + '_' + build
				regex = alias + '$'
				t = []
				for src in sourceTargets:
					if re.search(regex, str(src)):
						t.append(src)
				if len(t) != 0:
					targetsGroup += env.Alias(alias, t)

	if len(targetsGroup) != 0:
		targets += targetsGroup
		targets.append('')
		targetsGroup = []

	###############
	# platform_abi

	for platform in platforms:
		for abi in _getABIs(platform):
			alias = platform + '_' + abi
			t = []
			for build in builds:
				regex = platform + '_' + abi + '_' + build + '$'
				for src in sourceTargets:
					if re.search(regex, str(src)):
						t.append(src)
			if len(t) != 0:
				targetsGroup += env.Alias(alias, t)

	if len(targetsGroup) != 0:
		targets += targetsGroup
		targets.append('')
		targetsGroup = []

	###############
	# platform_build

	t = {}
	for platform in platforms:
		for build in builds:
			t[platform + '_' + build] = []
	for platform in platforms:
		for abi in _getABIs(platform):
			for build in builds:
				regex = platform + '_' + abi + '_' + build + '$'
				for src in sourceTargets:
					if re.search(regex, str(src)):
						t[platform + '_' + build].append(src)
	for platform_build in t.keys():
		l = t[platform_build]
		if len(l) != 0:
			targetsGroup += env.Alias(platform_build, l)

	if len(targetsGroup) != 0:
		targets += targetsGroup
		targets.append('')
		targetsGroup = []

	###############
	# platform

	t = {} 
	for platform in platforms:
		t[platform] = []
		for abi in _getABIs(platform):
			alias = platform + '_' + abi
			for build in builds:
				regex = platform + '_' + abi + '_' + build + '$'
				for src in sourceTargets:
					if re.search(regex, str(src)):
						t[platform].append(src)
	for platform in t.keys():
		l = t[platform]
		if len(l) != 0:
			targetsGroup += env.Alias(platform, l)

	if len(targetsGroup) != 0:
		targets += targetsGroup
		targets.append('')
		targetsGroup = []

	###############
	# build

	t = {}
	for build in builds:
		t[build] = []
	for platform in platforms:
		for abi in _getABIs(platform):
			for build in builds:
				regex = platform + '_' + abi + '_' + build + '$'
				for src in sourceTargets:
					if re.search(regex, str(src)):
						t[build].append(src)
	for build in t.keys():
		l = t[build]
		if len(l) != 0:
			targetsGroup += env.Alias(build, l)

	if len(targetsGroup) != 0:
		targets += targetsGroup
		targets.append('')
		targetsGroup = []

	###############
	# all

	targets += env.Alias('all', sourceTargets)

	###############

	for t in targets:
		s += '%s\n' % t

	env.Help(s)
	
	return targets


###############################################################################

