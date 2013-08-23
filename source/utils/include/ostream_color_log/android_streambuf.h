/**
 * @file android_streambuf.h
 * @date Mar 18, 2013
 *
 * @brief
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license GPLv3
 *
 */

#ifndef ANDROID_STREAMBUF_H_
#define ANDROID_STREAMBUF_H_

#ifdef __ANDROID__

///////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <android/log.h>

///////////////////////////////////////////////////////////////////////////////

namespace ostream_color_log {

	enum android_log_level{
	    DEFAULT = ANDROID_LOG_DEFAULT,
	    VERBOSE = ANDROID_LOG_VERBOSE,
	    DEBUG = ANDROID_LOG_DEBUG,
	    INFO = ANDROID_LOG_INFO,
	    WARN = ANDROID_LOG_WARN,
	    ERROR = ANDROID_LOG_ERROR,
	    FATAL = ANDROID_LOG_FATAL,
	    SILENT = ANDROID_LOG_SILENT
	};

	class android_streambuf: public std::streambuf{
	public:
		android_streambuf(android_log_level logLevel);

		virtual ~android_streambuf();

		void setLogTag(const std::string& logTag);

		virtual int sync();
		virtual std::streamsize xsputn(const char* s, std::streamsize n);
		virtual std::streambuf::int_type overflow(
				std::streambuf::int_type ch);

	private:
		android_log_level _logLevel;
		std::string _logTag;
		std::ostringstream _oss;
	};

} /* namespace ostream_color_log */

///////////////////////////////////////////////////////////////////////////////

#endif

#endif /* ANDROID_STREAMBUF_H_ */
