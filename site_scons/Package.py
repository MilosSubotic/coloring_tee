'''
@file: Builders.py
@date: Aug 14, 2013

@brief: Package builder.
@version: 1.2

@author: Milos Subotic milos.subotic.sm@gmail.com
@license: MIT
'''

###############################################################################

from __future__ import print_function

###############################################################################

__date__ = '2013/8/14 08:09:00'

__version__ = '1.0'
__license__ = 'MIT'

__author__ = 'Milos Subotic milos.subotic.sm@gmail.com'
__status__ = 'Beta'

###############################################################################

from SCons.Script import *

import Utils
import Targets
import Dirs

# Import OrderedDict
try:
	from collections import OrderedDict
except ImportError:
	# Python before 2.7
	try:
		from ordereddict import OrderedDict
	except ImportError:
		raise ImportError(
'''Cannot find OrderedDict in collections, nor can I find ordereddict. Try to 
install ordereddict with pip, like this: 

	sudo pip install ordereddict

If you do not have pip installed install it with:

	sudo apt-get install python-pip

''')

###############################################################################

class PackageBuildParams(Targets.CompileParams):
	def __init__(self, compileParams, packageName):
		# Remove later.
		Targets.CompileParams.__init__(self, compileParams.env, 
				compileParams.sharedEnv, compileParams.platform, 
				compileParams.abi, compileParams.build)
		self.packageName = packageName


class BuildInterface(object):

	def __init__(self):
		variableKeys = [ 'CXXFLAGS', 'CFLAGS', 'CPPFLAGS', 'LIBS', 'LIBPATH',
				'STATIC_LIBS', 'WHOLE_STATIC_LIBS', 'PACKAGE_BUILD_TARGET' ]
		# TODO Clone it from somewhere.
		self._variables = {}
		for v in variableKeys:
			self[v] = []
			
	def __getitem__(self, key):
		return self._variables[key]

	def __setitem__(self, key, value):
		self._variables[key] = value
		
	def __iadd__(self, other):
		for k, v in other._variables.items():
			if self._variables.has_key(k):
				if k == 'STATIC_LIBS' or k == 'WHOLE_STATIC_LIBS':
					nonUnique = self._variables[k] + v
					unique = OrderedDict.fromkeys(nonUnique).keys()
					self._variables[k] = unique
				else:
					self._variables[k] += v
			else:
				self._variables[k] = v
		return self

	def keys(self):
		return self._variables.keys()
		
	def items(self):
		return self._variables.items()
				
	def appendToEnv(self, env):
		for k, v in self._variables.items():
			if env.has_key(k):
				env[k] += v
			else:
				env[k] = v		
		
class PackageInterface(object):
	def __init__(self):
		self._builds = {}
	
	def __getitem__(self, platformAbiBuild):
		return self._builds[platformAbiBuild]

	def __add__(self, other):
		return [self, other]

	def keys(self):
		return self._builds.keys()
		
	def items(self):
		return self._builds.items()
	
class Package(object):

	def __init__(self, 
			project, 
			packageName, 
			publicPackage = False,
			withVariantDir = True,
			dependecies = []):
		self.project = project
		self.packageName = packageName
		self.publicPackage = publicPackage
		self.withVariantDir = withVariantDir
		# List of PackageInterface.
		if not dependecies:
			self.dependecies = []
		elif not SCons.Util.is_List(dependecies):
			self.dependecies = [ dependecies ]
		else:
			self.dependecies = dependecies

		self.sharedEnv = self.project.globalEnv.clone()
		self.targetFilter = self.project.targetFilter
		
		self.packageInterface = PackageInterface()
		

	def buildTarget(self, buildParams):
		'''
		Override to build target.
		@return: target
		'''
		return None

	def setUpBuildInterface(self, buildInterface, buildParams):
		'''
		Override to set up build interface.
		'''
		pass

	def crossCompile(self):

		###################################

		def _buildFunction(compileParams):
			buildParams = PackageBuildParams(compileParams = compileParams, 
					packageName = self.packageName)
			buildParams.withVariantDir = self.withVariantDir

			env = buildParams.env
			platformAbiBuild = buildParams.platformAbiBuild
						
			# Merge all BuildInterfaces of dependecies to one.
			dependecyBuildInterface = BuildInterface()
			for dependecy in self.dependecies:
				dependecyBuildInterface += dependecy[platformAbiBuild]
			dependecyBuildInterface.appendToEnv(env)
			buildParams.dependecyBuildInterface = dependecyBuildInterface
			
			buildParams.prerequisites = self.createPrerequisites(
					dependecyBuildInterface)
						
			buildParams.dirs = self.createDirs(buildParams = buildParams)
			
			if self.withVariantDir:
				buildParams.env.VariantDir(buildParams.dirs.packageBuildDir, 
						".",  duplicate = 0)
					
			buildInterface = BuildInterface()
			self.packageInterface._builds[platformAbiBuild] = buildInterface
			
			target = self.buildTarget(buildParams = buildParams)
			
			self.setUpBuildInterface(buildInterface, buildParams)
			
			buildInterface['PACKAGE_BUILD_TARGET'] = target
			
			return target
			
		###################################

		# Maybe move this here.
		self.targets = Targets.buildCrossCompileTargets(
				sharedEnv = self.sharedEnv,
				packageName = self.packageName,
				buildFunction = _buildFunction,
				targetFilter = self.targetFilter)

	def createDirs(self, buildParams):
		'''
		Override to customize dirs creation.
		'''
		return Dirs.Dirs(env = buildParams.env, 
				packageName = buildParams.packageName, 
				platformAbiBuild = buildParams.platformAbiBuild,
				withVariantDir = buildParams.withVariantDir)

	###################################

	def createPrerequisites(self, dependecyBuildInterface):
		# If dependecy is not under project dir, do not build it if exists.
		# TODO If VPATH is not under project dir than this will not work the 
		# same. Maybe check is it external package script.
		dependecies = []
		for dep in dependecyBuildInterface['PACKAGE_BUILD_TARGET']:
			if Utils.isUnderProjectDir(dep):
				dependecies.append(dep)
			elif not dep.exists():
				dependecies.append(dep)
		return dependecies


	def standardAddForDistclean(self):
		# Add dirs for distclean.
		if Utils.isUnderProjectDir(self.sharedEnv.getVPATH()):
			self.sharedEnv.addForDistclean(self.sharedEnv.getVPATH())
		# Is out dir under project dir.
		if Utils.isUnderProjectDir(self.sharedEnv.getPREFIX()):
			self.sharedEnv.addForDistclean(self.sharedEnv.getPREFIX())

	def addStandardTargets(self):
		# Standard targets.
		Targets.makeStandardTargets(self.sharedEnv, self.targets)
		self.sharedEnv.Default('all')

	def build(self):
		self.crossCompile()
		if self.sharedEnv.isRootPackage():
			self.addStandardTargets()
		self.standardAddForDistclean()
		
		if self.publicPackage:
			self.project._returnPackageInterfaces[self.packageName] = \
					self.packageInterface
		return self.packageInterface

	def addForDistclean(self, targets):
		self.sharedEnv.addForDistclean(targets)



class MakePackage(Package):
	def __init__(self, 
			project, 
			packageName,
			packageBuilderContructor,
			tarball,
			dirInTarball,
			patchFiles = [],
			configureFlags = '',
			makeFlags = '',
			shellFlags = '',
			targets = None,
			includeSubDirs = [],
			libs = [],
			publicPackage = False,
			withVariantDir = True,
			dependecies = []):
		Package.__init__(self, 
				project = project,
				packageName = packageName,
				publicPackage = publicPackage,
				withVariantDir = withVariantDir,
				dependecies = dependecies)
		self.packageBuilderContructor = packageBuilderContructor
		class PackageBuilderConstructorArguments:
			pass
		self.packageBuilderConstructorArguments = \
				PackageBuilderConstructorArguments()
		self.packageBuilderConstructorArguments.tarball = tarball
		self.packageBuilderConstructorArguments.dirInTarball = dirInTarball
		self.packageBuilderConstructorArguments.patchFiles = patchFiles
		self.packageBuilderConstructorArguments.configureFlags = configureFlags
		self.packageBuilderConstructorArguments.makeFlags = makeFlags
		self.packageBuilderConstructorArguments.shellFlags = shellFlags
		self.packageBuilderConstructorArguments.targets = targets

		if includeSubDirs:
			self.includeSubDirs = includeSubDirs
		else:
			self.includeSubDirs = []		
		if libs:
			self.libs = libs
		else:
			self.libs = [ self.packageName ]
		
	def buildTarget(self, buildParams):
		env = buildParams.env
		
		pbca = self.packageBuilderConstructorArguments
		self.packageBuilderInstance = self.packageBuilderContructor(
				buildParams = buildParams,
				tarball = pbca.tarball,
				dirInTarball = pbca.dirInTarball,
				patchFiles = pbca.patchFiles,
				configureFlags = pbca.configureFlags,
				makeFlags = pbca.makeFlags,
				shellFlags = pbca.shellFlags,
				targets = pbca.targets,
				prerequisites = buildParams.prerequisites
		)
		return self.packageBuilderInstance.run()
			
	def setUpBuildInterface(self, buildInterface, buildParams):
		dirs = buildParams.dirs
		includePath = dirs.packageOutDir.Dir('include')
		buildInterface['CPPPATH'] = [ includePath.abspath ] + \
				[ includePath.Dir(s).abspath for s in self.includeSubDirs ]
	
		dependecyBuildInterface = buildParams.dependecyBuildInterface
				
		libPath = dirs.packageOutDir.Dir('lib')
		buildInterface['LIBPATH'] = [ libPath.abspath ] + \
				dependecyBuildInterface['LIBPATH']
		buildInterface['LIBS'] = self.libs + \
				dependecyBuildInterface['LIBS']
		buildInterface['WHOLE_STATIC_LIBS'] = \
				[ libPath.File(Targets.staticLibraryName(
						buildParams.env, s)).abspath for s in self.libs ] + \
				dependecyBuildInterface['WHOLE_STATIC_LIBS']
		buildInterface['STATIC_LIBS'] = \
				dependecyBuildInterface['STATIC_LIBS']
				
class AutotoolsPackage(MakePackage):
	pass

#######################################

class SourcePackage(Package):
	def __init__(self, 
			project, 
			packageName,
			preferStaticLinking = True,
			publicPackage = False,
			withVariantDir = True,
			exportedIncludeDirs = [],
			libs = [],
			dependecies = []):
		Package.__init__(self, 
				project = project, 
				packageName = packageName, 
				publicPackage = publicPackage,
				withVariantDir = withVariantDir,
				dependecies = dependecies)
		self.preferStaticLinking = preferStaticLinking
		self.exportedIncludeDirs = map(Dir, exportedIncludeDirs)
		self.sharedEnv['CPPPATH'] += map(str, exportedIncludeDirs)
		if libs:
			self.libs = libs
		else:
			self.libs = [ self.packageName ]

	def collectSources(self):
		return Utils.glob('*.cpp', 'src/') + Utils.glob('*.c', 'src/')

class ProgramPackage(SourcePackage):
	def __init__(self, 
			project, 
			packageName,
			preferStaticLinking = True,
			publicPackage = False,
			withVariantDir = True,
			exportedIncludeDirs = [],
			libs = [],
			dependecies = []):
		SourcePackage.__init__(self, 
				project = project, 
				packageName = packageName,
				preferStaticLinking = preferStaticLinking,
				publicPackage = publicPackage,
				withVariantDir = withVariantDir,
				exportedIncludeDirs = exportedIncludeDirs,
				libs = libs,
				dependecies = dependecies)

	def buildTarget(self, buildParams):
		env = buildParams.env
		buildDir = buildParams.dirs.packageBuildDir
		
		src = self.collectSources()
		objs = [ buildDir.File(str(s)) for s in src ]
		
		env.Depends(objs, buildParams.prerequisites)
		
		dependecyBuildInterface = buildParams.dependecyBuildInterface
		if self.preferStaticLinking:
			# Link statically all static libraries.
			env['LINKFLAGS'] += [ '-Wl,-Bsymbolic', '-Wl,--whole-archive' ] + \
					dependecyBuildInterface['WHOLE_STATIC_LIBS'] + \
					[ '-Wl,--no-whole-archive' ] + \
					dependecyBuildInterface['STATIC_LIBS']
		else:
			# Set RPATH to all shared libraries.	
			env['RPATH'] = Utils.listToString(':', 
					dependecyBuildInterface['LIBPATH'])
		
		target = buildDir.File(Targets.programName(env,
				buildParams.packageName))
		return env.Program(target, objs)


class SharedLibraryPackage(SourcePackage):
	def __init__(self, 
			project, 
			packageName,
			preferStaticLinking = True,
			publicPackage = False,
			withVariantDir = True,
			exportedIncludeDirs = [],
			libs = [],
			dependecies = []):
		SourcePackage.__init__(self, 
				project = project, 
				packageName = packageName,
				preferStaticLinking = preferStaticLinking,
				publicPackage = publicPackage,
				withVariantDir = withVariantDir,
				exportedIncludeDirs = exportedIncludeDirs,
				libs = libs,
				dependecies = dependecies)

	def buildTarget(self, buildParams):
		env = buildParams.env
		buildDir = buildParams.dirs.packageBuildDir
		
		src = self.collectSources()
		objs = [ buildDir.File(str(s)) for s in src ]
		
		env.Depends(objs, buildParams.prerequisites)
		
		dependecyBuildInterface = buildParams.dependecyBuildInterface
		if self.preferStaticLinking:
			# Link statically all static libraries.
			env['LINKFLAGS'] += [ '-Wl,-Bsymbolic', '-Wl,--whole-archive' ] + \
					dependecyBuildInterface['WHOLE_STATIC_LIBS'] + \
					[ '-Wl,--no-whole-archive' ] + \
					dependecyBuildInterface['STATIC_LIBS']
		else:
			# Set RPATH to all shared libraries.	
			env['RPATH'] = Utils.listToString(':', 
					dependecyBuildInterface['LIBPATH'])
		
		target = buildDir.File(Targets.sharedLibraryName(env,
				buildParams.packageName))
		return env.SharedLibrary(target, objs)

	def setUpBuildInterface(self, buildInterface, buildParams):
		dirs = buildParams.dirs
		buildInterface['CPPPATH'] = map(
			lambda d: d.abspath, 
			self.exportedIncludeDirs
		)

		dependecyBuildInterface = buildParams.dependecyBuildInterface
				
		libPath = dirs.packageBuildDir
		buildInterface['LIBPATH'] = [ libPath.abspath ] + \
				dependecyBuildInterface['LIBPATH']
		buildInterface['LIBS'] = self.libs + \
				dependecyBuildInterface['LIBS']
		buildInterface['WHOLE_STATIC_LIBS'] = \
				dependecyBuildInterface['WHOLE_STATIC_LIBS']
		buildInterface['STATIC_LIBS'] = \
				dependecyBuildInterface['STATIC_LIBS']


class StaticLibraryPackage(SourcePackage):
	def __init__(self, 
			project, 
			packageName,
			preferStaticLinking = True,
			publicPackage = False,
			withVariantDir = True,
			exportedIncludeDirs = [],
			libs = [],
			dependecies = []):
		SourcePackage.__init__(self, 
				project = project, 
				packageName = packageName,
				preferStaticLinking = preferStaticLinking,
				publicPackage = publicPackage,
				withVariantDir = withVariantDir,
				exportedIncludeDirs = exportedIncludeDirs,
				libs = libs,
				dependecies = dependecies)

	def buildTarget(self, buildParams):
		env = buildParams.env
		buildDir = buildParams.dirs.packageBuildDir
		
		src = self.collectSources()
		objs = [ buildDir.File(str(s)) for s in src ]
		
		env.Depends(objs, buildParams.prerequisites)
		
		target = buildDir.File(Targets.staticLibraryName(env,
				buildParams.packageName))
		return env.StaticLibrary(target, objs)

	def setUpBuildInterface(self, buildInterface, buildParams):
		dirs = buildParams.dirs
		buildInterface['CPPPATH'] = map(
			lambda d: d.abspath, 
			self.exportedIncludeDirs
		)
	
		dependecyBuildInterface = buildParams.dependecyBuildInterface
				
		libPath = dirs.packageBuildDir
		buildInterface['LIBPATH'] = [ libPath.abspath ] + \
				dependecyBuildInterface['LIBPATH']
		buildInterface['LIBS'] = self.libs + \
				dependecyBuildInterface['LIBS']
		buildInterface['WHOLE_STATIC_LIBS'] = \
				[ libPath.File(Targets.staticLibraryName(buildParams.env, 
						buildParams.packageName)).abspath ] + \
				dependecyBuildInterface['WHOLE_STATIC_LIBS']
		buildInterface['STATIC_LIBS'] = \
				dependecyBuildInterface['STATIC_LIBS']

###############################################################################

