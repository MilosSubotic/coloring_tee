/**
 * @file ostream_coloring_streambuf.h
 * @date May 1, 2013
 *
 * @brief
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license LGPLv3
 *
 */

#ifndef OSTREAM_COLORING_STREAMBUF_H_
#define OSTREAM_COLORING_STREAMBUF_H_

///////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include "ostream_coloring.h"

///////////////////////////////////////////////////////////////////////////////

namespace ostream_color_log {

	class ostream_coloring_streambuf : public std::streambuf {
	public:
		ostream_coloring_streambuf(ostream_colors color,
				bool bold = true, std::ostream& outputOstream = std::cout);

		virtual ~ostream_coloring_streambuf();

		/**
		 * Enable or disable coloring. Coloring enabled by default.
		 * @param enableColoring
		 */
		void setColoringEnabled(bool coloringEnabled);
		void setBold(bool bold);

		/**
		 * Set output ostream in which log are going to be written.
		 * @param outputOstream
		 */
		void setOutputOstream(std::ostream& outputOstream);

		///////////////////////////////

	public:
		virtual int sync();
		virtual std::streamsize xsputn(const char* s, std::streamsize n);
		virtual std::streambuf::int_type overflow(
				std::streambuf::int_type ch);

		///////////////////////////////

	protected:
		bool _coloringEnabled;
		ostream_colors _color;
		bool _bold;
		std::ostream* _outputOstream;
		std::ostringstream _oss;
	};

} // namespace ostream_color_log

///////////////////////////////////////////////////////////////////////////////

#endif // OSTREAM_COLORING_STREAMBUF_H_
