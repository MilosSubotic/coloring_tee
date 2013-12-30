'''
@file: SConstruct
@date: Dec 7, 2012

@brief: Cross compilation SConstruct for coloring_tee project.
@version: 3.0 

@author: Milos Subotic milos.subotic.sm@gmail.com
@license: MIT
'''

###############################################################################

import Project
import Utils

###############################################################################

def targetFilter(buildParams):
	return buildParams.platform == Utils.hostPlatform() \
		and buildParams.abi == Utils.hostABI()

if Utils.hostPlatform() == 'linux':
	PREFIX = Dir('/usr/local')

project = Project.Project(
	projectName = Utils.getCurrentDirName(), 
	targetFilter = targetFilter,
	defaultVPATH = Dir('#/build').abspath,
	defaultPREFIX = PREFIX
)

project.buildPackages(Dir('source/coloring_tee'))

project.finish()

###############################################################################

