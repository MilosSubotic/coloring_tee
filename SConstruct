'''
@file: SConstruct
@date: Dec 7, 2012

@brief: Cross compilation SConstruct for coloring_tee project.
@version: 2.0 

@author: Milos Subotic milos.subotic.sm@gmail.com
@license: MIT
'''

###############################################################################

import os

import Utils

###############################################################################

tarballRoot = './'
rootProjectName = 'coloring_tee'

###############################################################################

globalEnv = Utils.constructSConsEnvironment()

# This project do not have Android targets.
globalEnv['ANDROID_NDK'] = '.'

# For enabling linking to shared library.
globalEnv['CPPFLAGS'] += [ '-fPIC' ]

###############################################################################
# This project could be built only for linux.

def targetFilter(targetTriple):
	if targetTriple.platform == 'linux':
		return True
	else:
		return False
		
globalEnv.globalConfig.targetFilter = targetFilter

###############################################################################

Export('globalEnv')

###############################################################################

SConscript('source/utils/SConscript')
SConscript('source/coloring_tee/SConscript')

Utils.addDistcleanTargets(globalEnv)
Utils.addTarballTarget(globalEnv, rootProjectName)

