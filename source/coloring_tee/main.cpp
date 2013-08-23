/**
 * @file main.cpp
 * @date Dec 7, 2012
 *
 * @brief Like tee but also can color console output or html based on scheme.
 * @version 2.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license GPLv3
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <csignal>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <cerrno>
using namespace std;

#include "ostream_color_log/ostream_coloring.h"
#include "ostream_color_log/html_ofstream.h"
using namespace ostream_color_log;

#include "LuaConfig.h"

#include "options.h"

///////////////////////////////////////////////////////////////////////////////

#define USER_CONFIG_DIR_NAME ".coloring_tee"
#define USER_CONFIG_FILE_NAME "config.lua"

///////////////////////////////////////////////////////////////////////////////

static const char* versionString =
"License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n"
"This is free software: you are free to change and redistribute it.\n"
"There is NO WARRANTY, to the extent permitted by law.\n"
"\n"
"Written by Milos Subotic.";

static vector<ofstream*> files;
static vector<html_ofstream*> htmlFiles;

///////////////////////////////////////////////////////////////////////////////

class String2OstreamColors{
private:
	map<string, ostream_colors> lookupTable;
public:
	String2OstreamColors(){
		lookupTable["black"] = black;
		lookupTable["red"] = red;
		lookupTable["green"] = green;
		lookupTable["yellow"] = yellow;
		lookupTable["blue"] = blue;
		lookupTable["magenta"] = magenta;
		lookupTable["cyan"] = cyan;
		lookupTable["white"] = white;
	}

	ostream_colors operator[](const string& s){
		return lookupTable[s];
	}
};

class SearchStringToColor{
public:
	std::string searchString;
	ostream_colors color;

	static String2OstreamColors _table;

public:
	SearchStringToColor(const std::string& searchString_,
			const std::string& color_)
			: searchString(searchString_), color(_table[color_]){
	}
};

String2OstreamColors SearchStringToColor::_table;

///////////////////////////////////////////////////////////////////////////////

static void cleanUp(int returnCode) __attribute__((noreturn));
static void cleanUp(int returnCode){
	for(int i = 0; i < files.size(); i++){
		files[i]->close();
		delete files[i];
	}
	for(int i = 0; i < htmlFiles.size(); i++){
		htmlFiles[i]->close();
		delete htmlFiles[i];
	}
	cout << flush;

	// Terminate program.
	exit(returnCode);
}

static void signalCallbackHandler(int signum){
	// Cleanup and close up stuff here.
	cleanUp(signum);
}

///////////////////////////////////////////////////////////////////////////////

static void checkUserConfigDir(const string& userConfigDirName){
	// rwxr-xr-x.
	mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
	if(mkdir(userConfigDirName.c_str(), mode)){
		// Did not create dir.
		if(errno == EEXIST){
			// Already exist, check what exist.
			struct stat s;
			if(!stat(userConfigDirName.c_str(), &s)){
				if(s.st_mode & S_IFDIR){
					// Dir exist, copy file.
					return;
				}else{
					// It is not dir.
					cerr << PROGRAM_NAME
							<< ": User configuration directory \""
							<< userConfigDirName
							<< "\" is not directory! Delete it so I "
							<< "could make valid directory!" << endl;
					cleanUp(-1);
				}
			}
		}else if(errno == EACCES){
			// No access to create.
			cerr << PROGRAM_NAME
					<< ": Do not have premission to create "
					<< "user configuration directory \""
					<< userConfigDirName << "\"!" << endl;
			cleanUp(-1);
		}else{
			cerr << PROGRAM_NAME
					<< ": Cannot create "
					<< "user configuration directory \""
					<< userConfigDirName << "\"!" << endl;
			cleanUp(-1);
		}
	}
	// Copy file.
}


static string checkUserConfigFile(){
	const char* home = getenv("HOME");
	if(!home){
		cerr << PROGRAM_NAME
				<< ": Cannot get \"HOME\" environment variable!"
				<< endl;
		cleanUp(-1);
	}
	ostringstream oss;
	oss << home << '/' << USER_CONFIG_DIR_NAME;
	string userConfigDirName(oss.str());
	oss << '/' << USER_CONFIG_FILE_NAME;
	string userConfigFileName(oss.str());

	if(access(userConfigFileName.c_str(), R_OK)){
		// There is no user config file lets make it.
		if(access(DEFAUL_CONFIG_FILE, R_OK)){
			cerr << PROGRAM_NAME
					<< ": Cannot access to default configuration file \""
					<< DEFAUL_CONFIG_FILE << "\"!" << endl;
			cleanUp(-1);
		}

		checkUserConfigDir(userConfigDirName);
		// At this point userConfigDirName is created, now copy file.

		ifstream src(DEFAUL_CONFIG_FILE);
		if(!src.is_open()){
			cerr << PROGRAM_NAME
					<< ": Cannot open user configuration file \""
					<< DEFAUL_CONFIG_FILE << "\" for reading!" << endl;
			cleanUp(-1);
		}
		ofstream dst(userConfigFileName.c_str());
		if(!dst.is_open()){
			cerr << PROGRAM_NAME
					<< ": Cannot open user configuration file \""
					<< userConfigFileName << "\" for writing!" << endl;
			cleanUp(-1);
		}
		dst << src.rdbuf();
		src.close();
		dst.close();

	}

	return userConfigFileName;
}

///////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv){

	// Skip program name argv[0] if present.
	argc -= (argc > 0);
	argv += (argc > 0);
	option::Stats stats(usage, argc, argv);
	option::Option options[stats.options_max], buffer[stats.buffer_max];
	option::Parser parse(usage, argc, argv, options, buffer);

	if(parse.error())
		return 1;

	if(options[HELP]){
		option::printUsage(std::cout, usage);
		return 0;
	}

	if(options[VERSION]){
		cout << PROGRAM_NAME" "VERSION_STR"\n"
				<< versionString << endl;
		return 0;
	}


	if(signal((int) SIGINT, &signalCallbackHandler) == SIG_ERR){
		cerr << PROGRAM_NAME
		<< ": Cannot connect interrupt signal handler!" << endl;
		cleanUp(-1);
	}

	LuaConfig config;
	string configFileName;
	const char* argConfigFileName = options[OPT_CONFIG_FILE].arg;
	if(argConfigFileName != NULL){
		if(access(argConfigFileName, R_OK)){
			cerr << PROGRAM_NAME << ": Cannot open configuration file \"" <<
					argConfigFileName << "\"!" << endl;
			cleanUp(-1);
		}else{
			configFileName = argConfigFileName;
		}
	}else{
		configFileName = checkUserConfigFile();
	}
	// FIXME If there is an error in config file this function gives
	// segmetation fault instead throwing exception.
	config.open(configFileName.c_str());


	bool append = options[APPEND];

	for(option::Option* opt = &options[HTML_OUTPUT]; opt; opt = opt->next()){
		if(!opt->arg){
			continue;
		}
		string fileName = opt->arg;

		if(fileName == "-"){
			continue;
		}

		html_ofstream* htmlFile;
		if(append){
			htmlFile = new html_ofstream(fileName.c_str(),
					ios_base::out | ios_base::app);
		}else{
			htmlFile = new html_ofstream(fileName.c_str(), ios_base::out);
		}

		if(!htmlFile->is_open()){
			cerr << PROGRAM_NAME << ": " << fileName << ": Permission denied"
					<< endl;
			delete htmlFile;
			continue;
		}

		htmlFiles.push_back(htmlFile);

	}

	for(int i = 0; i < parse.nonOptionsCount(); i++){
		string fileName = parse.nonOption(i);

		if(fileName == "-"){
			continue;
		}

		ofstream* file;
		if(append){
			file = new ofstream(fileName.c_str(),
					ios_base::out | ios_base::app);
		}else{
			file = new ofstream(fileName.c_str(), ios_base::out);
		}

		if(!file->is_open()){
			cerr << PROGRAM_NAME << ": " << fileName << ": Permission denied"
					<< endl;
			delete file;
			continue;
		}

		files.push_back(file);
	}

	bool coloringBold = !options[NO_BOLD];
	bool coloringEnabled = !options[NO_COLORS];

	vector<SearchStringToColor> searchStringToColor;

	if(coloringEnabled){
		config.getGlobalTable("coloring_tee_config");
		config.getFieldTable("color_schemes");

		// Get enabled color schemes from option flags.
		set<string> colorSchemes;
		const char* argColorSchemes = options[COLOR_SCHEMES].arg;
		if(argColorSchemes){
			string colorSchemesOption(argColorSchemes);
			istringstream iss(colorSchemesOption);
			// Remove = on begin of option string.
			if(iss.peek() == '='){
				char fooChar;
				iss.get(fooChar);
			}
			string item;
			while(getline(iss, item, ',')){
				colorSchemes.insert(item);
			}
		}

		for(config.iterationInit(); config.iterationCondition();
				config.iterationIncrement()){
			string colorScheme = config.getKeyAsString();
			// If color scheme is in options line, add color.
			if(colorSchemes.find(colorScheme) != colorSchemes.end()){
				for(lua_pushnil(config.L); lua_next(config.L, -2);
						lua_pop(config.L, 1)){
					searchStringToColor.push_back(
							SearchStringToColor(
									config.getFieldString("searchString"),
											config.getFieldString("color")));
				}
			}
		}

		config.pop();
		config.pop();
	}

	if(coloringEnabled){
		string line;
		while(getline(cin, line)){

			for(int i = 0; i < searchStringToColor.size(); i++){
				size_t found = line.find(
						searchStringToColor[i].searchString);
				if(found != string::npos){
					ostream_colors color = searchStringToColor[i].color;
					cout << color;
					for(int i = 0; i < htmlFiles.size(); i++){
						*htmlFiles[i] << color;
					}
					break;
				}
			}
			if(coloringBold){
				cout << bold;
				for(int i = 0; i < htmlFiles.size(); i++){
					*htmlFiles[i] << bold;
				}
			}

			cout << line << reset << endl;
			for(int i = 0; i < htmlFiles.size(); i++){
				*htmlFiles[i] << line << reset << std::endl;
			}

			for(int i = 0; i < files.size(); i++){
				*files[i] << line << endl;
			}

		}
	}else{
		string line;
		while(getline(cin, line)){

			cout << line << endl;

			for(int i = 0; i < htmlFiles.size(); i++){
				*htmlFiles[i] << line << std::endl;
			}

			for(int i = 0; i < files.size(); i++){
				*files[i] << line << endl;
			}

		}
	}

	cleanUp(0);
}

