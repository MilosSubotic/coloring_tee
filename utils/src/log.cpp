/**
 * @file log.cpp
 * @date Sep 19, 2012
 *
 * @brief
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license GPLv3
 *
 */

#include "log.h"

///////////////////////////////////////////////////////////////////////////////

#ifdef __ANDROID__

#include "ostream_color_log/android_streambuf.h"

using namespace ostream_color_log;

static android_streambuf _verboseStreambuf(VERBOSE);
static android_streambuf _debugStreambuf(DEBUG);
static android_streambuf _infoStreambuf(INFO);
static android_streambuf _warningStreambuf(WARN);
static android_streambuf _errorStreambuf(ERROR);

static std::ostream _verboseOstream(&_verboseStreambuf);
static std::ostream _debugOstream(&_debugStreambuf);
static std::ostream _infoOstream(&_infoStreambuf);
static std::ostream _warningOstream(&_warningStreambuf);
static std::ostream _errorOstream(&_errorStreambuf);

std::ostream& verboseLog = _verboseOstream;
std::ostream& debugLog = _debugOstream;
std::ostream& infoLog = _infoOstream;
std::ostream& warningLog = _warningOstream;
std::ostream& errorLog = _errorOstream;

void setLogTag(const std::string& logTag){
	_verboseStreambuf.setLogTag(logTag);
	_debugStreambuf.setLogTag(logTag);
	_infoStreambuf.setLogTag(logTag);
	_warningStreambuf.setLogTag(logTag);
	_errorStreambuf.setLogTag(logTag);
}

#else
#include <iostream>
std::ostream& verboseLog = std::cout;
std::ostream& debugLog = std::cout;
std::ostream& infoLog = std::cout;
std::ostream& warningLog = std::cerr;
std::ostream& errorLog = std::cerr;
#endif


///////////////////////////////////////////////////////////////////////////////

