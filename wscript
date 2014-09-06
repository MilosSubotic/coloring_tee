#! /usr/bin/env python
# encoding: utf-8

'''
@file: wscript
@date: Sep 6, 2014

@author: Milos Subotic <milos.subotic.sm@gmail.com>
@license: MIT

@brief: Waf script for building coloring_tee project.

@version: 1.0
Changelog:
1.0 - Initial version.

'''

###############################################################################

import os
import fnmatch
import shutil

###############################################################################

VERSION = '4.0'
APPNAME = 'coloring_tee'

top = '.'
out = 'build'

###############################################################################

def recursive_glob(pattern, directory = '.'):
	for root, dirs, files in os.walk(directory, followlinks = True):
		for f in files:
			if fnmatch.fnmatch(f, pattern):
				yield os.path.join(root, f)
		for d in dirs:
			if fnmatch.fnmatch(d + '/', pattern):
				yield os.path.join(root, d)

def collect_git_ignored_files():
	for gitignore in recursive_glob('.gitignore'):
		with open(gitignore) as f:
			base = os.path.dirname(gitignore)
			
			for pattern in f.readlines():
				pattern = pattern[:-1]
				for f in recursive_glob(pattern, base):
					yield f

###############################################################################

def options(opt):
	opt.load('compiler_c compiler_cxx')

	opt.add_option(
		'--debug',
		action = 'store_true',
		dest = 'debug',
		default = False, 
		help = 'enable compiler debug information'
	)

	opt.recurse('source')


def configure(conf):
	conf.load('compiler_c compiler_cxx')

	conf.env.CFLAGS += []
	conf.env.CXXFLAGS += []

	if conf.options.debug:
		conf.env.CFLAGS.append('-g')
		conf.env.CXXFLAGS.append('-g')


	conf.define('INSTALL_PREFIX', conf.options.prefix)
	conf.define('VERSION_STR', VERSION)
	conf.define('PROGRAM_NAME', APPNAME)
	conf.define(
		'DEFAUL_CONFIG_FILE', 
		os.path.join(conf.options.prefix, 'share/coloring_tee/config.lua')
	)
	conf.write_config_header('config.h')

	conf.recurse('source')

def build(bld):
	bld.install_files(
		'${PREFIX}/share/coloring_tee', 
		bld.path.ant_glob('share/coloring_tee/*')
	)
	bld.install_files(
		'${PREFIX}/bin', 
		bld.path.ant_glob('bin/*'),
		chmod = 0755
	)

	bld.recurse('source')

def distclean(ctx):
	for fn in collect_git_ignored_files():
		if os.path.isdir(fn):
			shutil.rmtree(fn)
		else:
			os.remove(fn)

def dist(ctx):
	# TODO For now dist is with .git, make new command backup.
	
	now = datetime.datetime.now()
	time_stamp = '{:d}-{:02d}-{:02d}-{:02d}-{:02d}-{:02d}'.format(
		now.year,
		now.month,
		now.day,
		now.hour,
		now.minute,
		now.second
	)
	ctx.arch_name = '../{}-{}.zip'.format(APPNAME, time_stamp)
	ctx.algo = 'zip'
	ctx.base_name = APPNAME
	# Also pack git.
	waflib.Node.exclude_regs = waflib.Node.exclude_regs.replace(
'''
**/.git
**/.git/**
**/.gitignore''', '')

###############################################################################

