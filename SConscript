'''
@file: SConscript
@date: Okt 20, 2012

@brief: Cross compilation SConscript.
@version: 2.0 

@author: Milos Subotic milos.subotic.sm@gmail.com
@license: MIT
'''

###############################################################################

from __future__ import print_function

import os
import sys
import Utils
import Targets
import ProjectCompile
import ShellHelpers

###############################################################################

Import('globalEnv')

localEnv = Utils.cloneSConsEnvironment(globalEnv)

###############################################################################

projectName = 'coloring_tee'

version = '2.0'

prefix = Dir(localEnv['PREFIX'])

configFileName = 'config.lua'

sourceConfigFile = Dir('#').Dir('share/coloring_tee').File(configFileName)

defaultConfigDir = prefix.Dir('share/coloring_tee')
defaultConfigFile = defaultConfigDir.File(configFileName)

###############################################################################

# TODO This fails in concatenating string and define of string.
# Also find out how to add -std=c++11 instead of -std=c++0x.
#localEnv['CXXFLAGS'] += localEnv.globalConfig.cxx0xCXXFLAGS()

###############################################################################

# Do not config when clean or when tarballing.
# That wont influence on flags which are set by these lines.
if not (localEnv.GetOption('clean') or (
			'tarball' in COMMAND_LINE_TARGETS)):

	# Check for pkg-config and lua5.1.

	def CheckPKGConfig(context, version):
		context.Message('Checking for pkg-config... ')
		ret = context.TryAction(
				'pkg-config --atleast-pkgconfig-version=%s' % version)[0]
		context.Result(ret)
		return ret

	def CheckPKG(context, name):
		context.Message('Checking for %s... ' % name)
		ret = context.TryAction('pkg-config --exists \'%s\'' % name)[0]
		context.Result(ret)
		return ret


	conf = Configure(localEnv, custom_tests =
					{ 'CheckPKGConfig' : CheckPKGConfig,
					'CheckPKG' : CheckPKG },
					conf_dir = localEnv.getVPATH().Dir('.sconf_temp'),
					log_file = localEnv.getVPATH().File('config.log'))

	if not conf.CheckPKGConfig('0.15.0'):
		print('pkg-config >= 0.15.0 not found.')
		Exit(1)


	if conf.CheckPKG('lua5.1'):
		luaName = 'lua5.1'
	elif conf.CheckPKG('lua'):
		luaName = 'lua'
	else:
		print('lua nor lua5.1 not found.')
		Exit(1)

	conf.Finish()
	
	###################################
	# lua5.1 flags.
	
	localEnv.ParseConfig('pkg-config --cflags --libs ' + luaName)

###############################################################################
# Building config.h...

def build_config_h(env, target, source):
	for a_target in target:
		print('Building {0}...'.format(a_target))
		with file(str(a_target), "w") as config_h:
			config_h.write('#define INSTALL_PREFIX "{0}"\n'.format(
					prefix.abspath))
			config_h.write('#define VERSION_STR "{0}"\n'.format(version))
			config_h.write('#define PROGRAM_NAME "{0}"\n'.format(projectName))
			config_h.write('#define DEFAUL_CONFIG_FILE "{0}"\n'.format(
					defaultConfigFile))

config_h = localEnv.getVPATH().File('config.h')
config_h = localEnv.Command(target = config_h,
		source = '', action = build_config_h)

# Used for config.
localEnv['CPPPATH'].append(localEnv.getVPATH().abspath)

###############################################################################

def _programBuildFunctionRecursive(projectCompileParams):

	env = projectCompileParams.clonedEnv

	###################################

	if projectCompileParams.platform == 'linux':

		###################################

		src = Utils.glob('*.cpp', 'src/')

		buildDir = projectCompileParams.portBuildDir
		src = [ buildDir.File(s) for s in src ]
		target = buildDir.File(ProjectCompile.programName(env,
			projectCompileParams.projectName))

		###################################

		projects = [ 'utils' ]

		env['CPPPATH'] += [ os.path.join(s, 'include/') for s in projects ]

		subProjects = [ buildDir.Dir(s).File(
				ProjectCompile.staticLibraryName(env, s)) for s in projects ]

		src += subProjects

		###################################

		return projectCompileParams.clonedEnv.Program(target, src)

###############################################################################

targets = ProjectCompile.crossCompile(env = localEnv,
		projectName = projectName,
		buildFunction = _programBuildFunctionRecursive,
		VPATH = localEnv.getVPATH())

#######################################

if localEnv.isRootProject():
	standardTargets = Targets.makeStandardTargets(localEnv, targets)
	localEnv.Default('linux_release')

	installTargets = []
	installTargets += localEnv.Install(prefix.Dir('bin'),
			localEnv.getVPATH().File('linux/x86_64/release/coloring_tee'))
	installTargets += localEnv.Install(defaultConfigDir,
			sourceConfigFile)
	installTargets += localEnv.Install(prefix.Dir('bin'),
			[ Dir('#').File(s) for s in Utils.glob('*', 'bin') ])

	localEnv.Alias('install', installTargets)
	localEnv.Help('\ninstall\n')


