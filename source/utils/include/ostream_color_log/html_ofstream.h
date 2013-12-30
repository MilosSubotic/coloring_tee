/**
 * @file html_ofstream.h
 * @date May 4, 2012
 *
 * @brief Ostream for writing text to files in HTML format.
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license LGPLv3
 *
 */

#ifndef HTML_OFSTREAM_H_
#define HTML_OFSTREAM_H_

///////////////////////////////////////////////////////////////////////////////

#include <ostream_color_log/html_filebuf.h>

///////////////////////////////////////////////////////////////////////////////

namespace ostream_color_log {

	///////////////////////////////////

	class html_ofstream : public std::ostream {
	public:
		html_ofstream();
		explicit html_ofstream(
				const char* name,
				ios_base::openmode mode = ios_base::out | ios_base::trunc,
				ostream_colors foreground = white,
				ostream_colors background = black,
				ostream_attributes attribute = nothing);
		~html_ofstream(){}

		void open(
				const char* name,
				ios_base::openmode mode = ios_base::out | ios_base::trunc,
				ostream_colors foreground = white,
				ostream_colors background = black,
				ostream_attributes attribute = nothing);


		void close();

		html_filebuf* rdbuf() const{
			return const_cast<html_filebuf*>(&_filebuf);
		}

		bool is_open() {
			return _filebuf.is_open();
		}
		bool is_open() const {
			return _filebuf.is_open();
		}

		///////////////////////////////

	public:

		///////////////////////////////

	protected:
		html_filebuf _filebuf;
	};

	template<typename Type>
	inline html_ofstream& operator<<(html_ofstream& hofs, const Type& value){
		static_cast<std::ostream&>(hofs) << value;
		return hofs;
	}

}// namespace ostream_color_log

///////////////////////////////////////////////////////////////////////////////
#endif /* HTML_OFSTREAM_H_ */
