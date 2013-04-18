'''
@file: ShellHelpers.py
@date: Okt 17, 2012

@brief: Utils and helper stuff for SConsScripts
@version: 2.0 

@author: Milos Subotic milos.subotic.sm@gmail.com
@license: MIT
'''

###############################################################################

from __future__ import print_function

import os
import sys
import shutil
import time
import subprocess

###############################################################################

def touch(fname):
	if os.path.exists(fname):
		os.utime(fname, None)
	else:
		open(fname, 'w').close()

def mkdir_p(path):
	try:
		os.makedirs(path)
	except OSError as exc: # Python > 2.5
		if exc.errno != errno.EEXIST:
			raise

# TODO Change this function with these below.
def run(cmd):
	'''Run a command and decipher the return code. Exit by default.'''
	print(cmd)
	res = os.system(cmd)
	if res != 0:
		raise OSError("Fail to run command '%s', return code: %d"
					% (cmd, res))

def runCommand(command):
	p = subprocess.Popen(command.split(), stdout = subprocess.PIPE,
						stderr = subprocess.PIPE)
	out, err = p.communicate()
	if err != '':
		 sys.stderr.write(err)
	return out

def runCommandForLines(command):
	p = subprocess.Popen(command.split(), stdout = subprocess.PIPE,
						stderr = subprocess.PIPE)
	err = p.stderr.read()
	if err != '':
		 sys.stderr.write(err)
	return p.stdout.readlines()

