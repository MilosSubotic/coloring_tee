'''
@file: Swig.py
@date: Okt 29, 2012

@brief: Build SWIG wrapper files.
@version: 2.0 

@author: Milos Subotic milos.subotic.sm@gmail.com
@license: MIT
'''
###############################################################################

from __future__ import print_function

from SCons.Script import *

import ShellHelpers
import re
import subprocess
import os

###############################################################################


def JavaSwig(env, swigSources, outDir, package):
	'''
	@return swigSourcesToWrapSources dictionary
	'''

	if not SCons.Util.is_List(swigSources):
		swigSources = [ swigSources ]

	rootDir = os.getcwd()
	javaDir = outDir.Dir('java').Dir(re.sub('\.', '/', package))
	
	swigSourcesToWrapSources = {}
	
	for swigSource in swigSources:
		target = re.sub('.i$', '_wrap.cxx', swigSource)
		target = outDir.Dir('capi').File(target)

		cmd = 'cd "{rootDir}"; swig -c++ -java -outdir "{javaDir}" ' \
				'-package "{package}" -o "{target}" "{swigSource}"'.format( 
				rootDir = rootDir, 
				javaDir = javaDir.abspath, 
				package = package, 
				target = target.abspath, 
				swigSource = swigSource )

		def _buildSwig(env, target, source):
			ShellHelpers.run(cmd)

		env.Command(target, swigSource, [
				Mkdir(javaDir),
				_buildSwig 
				]); 
				
		swigSourcesToWrapSources[swigSource] = target

	return swigSourcesToWrapSources
	
def buildSwigSources(env, swigSourcesToWrapSources, buildDir):

	objects = []
	
	for swigSource, wrapSource in swigSourcesToWrapSources.items():
		target = re.sub('.i$', '_wrap.o', swigSource)
		target = buildDir.Dir('capi').File(target)
		objects += env.Object(target = target, source = wrapSource)
		
	return objects
	
	
	
def getLinuxPlatformJavaIncludePath():

	cmd = [ 'update-alternatives', '--get-selections' ]
	out = subprocess.check_output(cmd)
	
	rows = out.split('\n')
	java = ''
	for row in rows:
		line = row.split()
		if line[0] == 'java':
			java = line[2]
			break
			
	return re.sub('jre/bin/java', 'include/', java)
	
	
	

