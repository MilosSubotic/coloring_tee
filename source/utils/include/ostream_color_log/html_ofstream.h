/**
 * @file html_ofstream.h
 * @date May 4, 2012
 *
 * @brief Ostream for writing text to files in HTML format.
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license GPLv3
 *
 */

#ifndef HTML_OFSTREAM_H_
#define HTML_OFSTREAM_H_

///////////////////////////////////////////////////////////////////////////////

#include <ostream_color_log/ostream_coloring.h>
#include <fstream>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////

namespace ostream_color_log {

	class html_ofstream;

	///////////////////////////////////
	// Manipulator interfaces for html_ofstream.

	// Coloring manipulators.
	html_ofstream& operator<<(html_ofstream& hofs,
			ostream_colors foreground);
	html_ofstream& operator<<(html_ofstream& hofs,
			ostream_attributes attribute);
	html_ofstream& operator<<(html_ofstream& hofs,
			_OstreamAttributeAndColorFormat format);

	///////////////////////////////////
	// Manipulator interfaces for html_ofstream.

	// Ostream manipulators.
	html_ofstream& flush(html_ofstream& hofs);
	html_ofstream& endl(html_ofstream& hofs);
	html_ofstream& ends(html_ofstream& hofs);

	///////////////////////////////////

	class html_ofstream: public std::ofstream{

		friend html_ofstream& operator<<(html_ofstream& hofs,
				ostream_colors foreground);
		friend html_ofstream& operator<<(html_ofstream& hofs,
				ostream_attributes attribute);
		friend html_ofstream& operator<<(html_ofstream& hofs,
				_OstreamAttributeAndColorFormat format);

		friend html_ofstream& flush(html_ofstream& hofs);
		friend html_ofstream& endl(html_ofstream& hofs);
		friend html_ofstream& ends(html_ofstream& hofs);

	protected:
		std::string _attributes, _foreground, _background;

	public:

		html_ofstream(const char* name, ios_base::openmode mode,
				_OstreamAttributeAndColorFormat format);

		html_ofstream(const char* name,
				ios_base::openmode mode = ios_base::out | ios_base::trunc,
				ostream_colors foreground = white, ostream_colors background =
						black, ostream_attributes attribute = nothing);

		void close();

		~html_ofstream(){
			close();
		}

		/**
		 * Interface for manipulators.
		 *  Manipulators such as @c std::endl and @c std::hex use these
		 *  functions in constructs like "std::cout << std::endl".
		 * @param pf manipulator.
		 * @return this
		 */
		html_ofstream& operator<<(html_ofstream& (*pf)(html_ofstream&)){
			return pf(*this);
		}

		template<typename DataType>
		html_ofstream& operator<<(const DataType& data){
			using namespace std;
			ostringstream oss;
			oss << data;
			htmlescapeTextArea(oss.str());
			return *this;
		}

	protected:
		std::ofstream* thisOfstream(){
			return reinterpret_cast<std::ofstream*>(this);
		}

	protected:
		void init(const char* name,
				_OstreamAttributeAndColorFormat format);
		static const char* makeColorString(ostream_colors color);
		void htmlescapeTextArea(const std::string& text);
		void writeAttributeAndColors();
		void setForeground(ostream_colors foreground);
		void setBackground(ostream_colors background);
		void setAttribute(ostream_attributes attribute);
	};

///////////////////////////////////

}// namespace ostream_color_log

///////////////////////////////////////////////////////////////////////////////

namespace std {

	// Ostream manipulators.
	inline ostream_color_log::html_ofstream& flush(
			ostream_color_log::html_ofstream& hofs){
		hofs << ostream_color_log::flush;
		return hofs;
	}

	inline ostream_color_log::html_ofstream& endl(
			ostream_color_log::html_ofstream& hofs){
		hofs << ostream_color_log::endl;
		return hofs;
	}

	inline ostream_color_log::html_ofstream& ends(
			ostream_color_log::html_ofstream& hofs){
		hofs << ostream_color_log::endl;
		return hofs;
	}

} // namespace std

///////////////////////////////////////////////////////////////////////////////

#endif /* HTML_OFSTREAM_H_ */
