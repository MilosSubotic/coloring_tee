/**
 * @file html_filebuf.h
 * @date Dec 30, 2013
 *
 * @author Milos Subotic <milos.subotic.sm@gmail.com>
 * @license MIT
 *
 * @brief
 *
 * @version 1.0
 * Changelog:
 * 1.0 - Initial version.
 *
 */

#ifndef HTML_FILEBUF_H_
#define HTML_FILEBUF_H_

///////////////////////////////////////////////////////////////////////////////

#include <fstream>

#include <ostream_color_log/ostream_coloring.h>

///////////////////////////////////////////////////////////////////////////////

namespace ostream_color_log {

	class html_ofstream;

	html_ofstream& operator<<(html_ofstream& hofs,
			ostream_colors foreground);
	html_ofstream& operator<<(html_ofstream& hofs,
			ostream_attributes attribute);
	html_ofstream& operator<<(html_ofstream& hofs,
			_OstreamAttributeAndColorFormat format);

	/**
	 * @class html_filebuf
	 * @brief
	 */
	class html_filebuf : public std::streambuf {
	public:
		// They need to have access for setting html_filebuf attributes.
		friend html_ofstream& operator<<(html_ofstream& hofs,
				ostream_colors foreground);
		friend html_ofstream& operator<<(html_ofstream& hofs,
				ostream_attributes attribute);
		friend html_ofstream& operator<<(html_ofstream& hofs,
				_OstreamAttributeAndColorFormat format);

	public:
		html_filebuf();
		virtual ~html_filebuf();

		///////////////////////////////

	public:

		html_filebuf* open(
				const char* name,
				std::ios_base::openmode mode,
				ostream_colors foreground,
				ostream_colors background,
				ostream_attributes attribute);

		html_filebuf* close();

		bool is_open() const{
			return _filebuf.is_open();
		}

		///////////////////////////////

	public:
		// TODO Add support for input.
		virtual int sync();
		virtual std::streamsize xsputn(const char* s, std::streamsize n);
		virtual std::streambuf::int_type overflow(
				std::streambuf::int_type ch);

		///////////////////////////////

	protected:
		std::streamsize directWrite(const char* s);
		std::streamsize directWrite(const std::string& s);
		static const char* makeColorString(ostream_colors color);
		void writeAttributeAndColors();
		void setForeground(ostream_colors foreground);
		void setBackground(ostream_colors background);
		void setAttribute(ostream_attributes attribute);

		///////////////////////////////

	protected:
		std::string _attributes;
		std::string _foreground;
		std::string _background;

		std::filebuf _filebuf;
	};

} // namespace ostream_color_log

///////////////////////////////////////////////////////////////////////////////

#endif // HTML_FILEBUF_H_
