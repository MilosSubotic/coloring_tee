/**
 * @file ostream_coloring_streambuf.cpp
 * @date May 1, 2013
 *
 * @brief
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license LGPLv3
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include "ostream_color_log/ostream_coloring_streambuf.h"

#include <cstdlib>

///////////////////////////////////////////////////////////////////////////////

namespace ostream_color_log {


	ostream_coloring_streambuf::ostream_coloring_streambuf(
			ostream_colors color, bool bold, std::ostream& outputOstream)
		: _coloringEnabled(true), _color(color), _bold(bold),
		  _outputOstream(&outputOstream){
	}

	ostream_coloring_streambuf::~ostream_coloring_streambuf(){
	}

	/**
	 * Enable or disable coloring. Coloring enabled by default if console
	 * support coloring, if not does nothing.
	 * @param enableColoring
	 */
	void ostream_coloring_streambuf::setColoringEnabled(bool coloringEnabled){
		_coloringEnabled = coloringEnabled;
	}

	void ostream_coloring_streambuf::setBold(bool bold){
		_bold = bold;
	}

	/**
	 * Set output ostream in which log are going to be written.
	 * @param outputOstream
	 */
	void ostream_coloring_streambuf::setOutputOstream(
			std::ostream& outputOstream){
		_outputOstream = &outputOstream;
	}

    /**
     *  @brief  Synchronizes the buffer arrays with the controlled sequences.
     *  @return  -1 on failure.
     *
     *  Each derived class provides its own appropriate behavior,
     *  including the definition of @a failure.
     *  @note  Base class version does nothing, returns zero.
    */
	int ostream_coloring_streambuf::sync(){
		if(_coloringEnabled){
			if(_bold){
				(*_outputOstream) << bold;
			}
			(*_outputOstream) << _color << _oss.str() << reset << std::flush;
		}else{
			(*_outputOstream) << _oss.str() << std::flush;
		}
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
	std::streamsize ostream_coloring_streambuf::xsputn(const char* s,
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
	std::streambuf::int_type ostream_coloring_streambuf::overflow(
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

} // namespace ostream_color_log

///////////////////////////////////////////////////////////////////////////////
