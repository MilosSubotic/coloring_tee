/**
 * @file android_streambuf.cpp
 * @date Mar 18, 2013
 *
 * @brief
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license GPLv3
 *
 */

#ifdef __ANDROID__

///////////////////////////////////////////////////////////////////////////////

#include "ostream_color_log/android_streambuf.h"

///////////////////////////////////////////////////////////////////////////////

namespace ostream_color_log {

	android_streambuf::android_streambuf(android_log_level logLevel)
				: _logLevel(logLevel){}

	void android_streambuf::setLogTag(const std::string& logTag){
		_logTag = logTag;
	}

	android_streambuf::~android_streambuf(){}

    /**
     *  @brief  Synchronizes the buffer arrays with the controlled sequences.
     *  @return  -1 on failure.
     *
     *  Each derived class provides its own appropriate behavior,
     *  including the definition of @a failure.
     *  @note  Base class version does nothing, returns zero.
    */
	int android_streambuf::sync(){
		__android_log_write(_logLevel, _logTag.c_str(), _oss.str().c_str());
		_oss.str("");
		return 0;
	}

    // Put area:
    /**
     *  @brief  Multiple character insertion.
     *  @param  s  A buffer area.
     *  @param  n  Maximum number of characters to write.
     *  @return  The number of characters written.
     *
     *  Writes @a s[0] through @a s[n-1] to the output sequence, as if
     *  by @c sputc().  Stops when either @a n characters have been
     *  copied, or when @c sputc() would return @c traits::eof().
     *
     *  It is expected that derived classes provide a more efficient
     *  implementation by overriding this definition.
    */
	std::streamsize android_streambuf::xsputn(const char* s,
			std::streamsize n){
		_oss.write(s, n);
		return n;
	}

    /**
     *  @brief  Consumes data from the buffer; writes to the
     *          controlled sequence.
     *  @param  c  An additional character to consume.
     *  @return  eof() to indicate failure, something else (usually
     *           @a c, or not_eof())
     *
     *  Informally, this function is called when the output buffer
     *  is full (or does not exist, as buffering need not actually
     *  be done).  If a buffer exists, it is @a consumed, with
     *  <em>some effect</em> on the controlled sequence.
     *  (Typically, the buffer is written out to the sequence
     *  verbatim.)  In either case, the character @a c is also
     *  written out, if @a c is not @c eof().
     *
     *  For a formal definition of this function, see a good text
     *  such as Langer & Kreft, or [27.5.2.4.5]/3-7.
     *
     *  A functioning output streambuf can be created by overriding only
     *  this function (no buffer area will be used).
     *
     *  @note  Base class version does nothing, returns eof().
    */
	std::streambuf::int_type android_streambuf::overflow(
				std::streambuf::int_type c)
	{
		if (c == traits_type::eof())
			return traits_type::eof();
		else
		{
			char_type ch = traits_type::to_char_type(c);
			return xsputn(&ch, 1) == 1 ? c : traits_type::eof();
		}
	}

} /* namespace ostream_color_log */

#endif

///////////////////////////////////////////////////////////////////////////////

