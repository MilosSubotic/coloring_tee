/**
 * @file options.cpp
 * @date Oct 17, 2012
 *
 * @brief
 * @version 2.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license GPLv3
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include "options.h"

#include <sstream>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////

static const char* concat(std::initializer_list<const char*> list) {
	std::ostringstream oss;
	for(const char* elem: list){
		oss << elem;
	}
	std::string s = oss.str();
	char* c = new char[s.size() + 1];
	strcpy(c, s.c_str());
	return c;
}

const option::Descriptor usage[] = {
    { UNKNOWN,           0,  "",                  "", option::Arg::None,     concat({"USAGE: ", PROGRAM_NAME, " [OPTION]... [FILE]...\n"
	                                                                          "Copy standard input to each FILE, and put colored text to standard output.\n"}) },
	{ APPEND,            0, "a",            "append", option::Arg::None,     "  -a, --append            \tappend to the given FILEs, do not overwrite" },
	{ IGNORE_INTERRUPTS, 0, "i", "ignore-interrupts", option::Arg::None,     "  -i, --ignore-interrupts \tignore interrupt signals" },
	{ HTML_OUTPUT,       0, "H",              "html", option::Arg::Optional, "  -H, --html              \toutput file in HTML format" },
	{ NO_COLORS,         0,  "",         "no-colors", option::Arg::None,     "      --no-coloring       \tno coloring" },
	{ NO_BOLD,           0,  "",           "no-bold", option::Arg::None,     "      --no-bold           \tno bold output" },
	{ COLOR_SCHEMES,     0, "c",     "color-schemes", option::Arg::Optional, "  -c, --color-schemes     \tcolor schemes, separeted with \",\"\n" },
	{ OPT_CONFIG_FILE,   0,  "",            "config", option::Arg::Optional, "      --config            \tconfiguration file" },
    { HELP,              0, "h",              "help", option::Arg::None,     "  -h, --help              \tdisplay this help and exit" },
    { VERSION,           0,  "",           "version", option::Arg::None,     "      --version           \toutput version information and exit" },

    { UNKNOWN,           0,  "",                  "", option::Arg::None,     concat({"\nIf a FILE is -, copy again to standard output."
                                                                              "\nBy default all logs colorings are enabled."
                                                                              "\nEnabling any of specific logs turns off all others.\n\n"
                                                                              "Report ", PROGRAM_NAME, " bugs to milos.subotic.sm@gmail.com\n"}) },
    { 0, 0, 0, 0, 0, 0 }
};


///////////////////////////////////////////////////////////////////////////////


