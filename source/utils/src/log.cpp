/**
 * @file log.cpp
 * @date Sep 19, 2012
 *
 * @brief
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license LGPLv3
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

void log_setLogTag(const std::string& logTag){
	_verboseStreambuf.setLogTag(logTag);
	_debugStreambuf.setLogTag(logTag);
	_infoStreambuf.setLogTag(logTag);
	_warningStreambuf.setLogTag(logTag);
	_errorStreambuf.setLogTag(logTag);
}

#else

#include "ostream_color_log/ostream_coloring_streambuf.h"

using namespace ostream_color_log;

static ostream_coloring_streambuf _verboseStreambuf(white);
static ostream_coloring_streambuf _debugStreambuf(blue);
static ostream_coloring_streambuf _infoStreambuf(green);
static ostream_coloring_streambuf _warningStreambuf(yellow);
static ostream_coloring_streambuf _errorStreambuf(red);

void log_setColoringEnabled(bool coloringEnabled){
	_verboseStreambuf.setColoringEnabled(coloringEnabled);
	_debugStreambuf.setColoringEnabled(coloringEnabled);
	_infoStreambuf.setColoringEnabled(coloringEnabled);
	_warningStreambuf.setColoringEnabled(coloringEnabled);
	_errorStreambuf.setColoringEnabled(coloringEnabled);
}

void log_setBold(bool bold){
	_verboseStreambuf.setBold(bold);
	_debugStreambuf.setBold(bold);
	_infoStreambuf.setBold(bold);
	_warningStreambuf.setBold(bold);
	_errorStreambuf.setBold(bold);
}

#endif

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

///////////////////////////////////////////////////////////////////////////////

