/**
 * @file html_ofstream.cpp
 * @date May 4, 2012
 *
 * @brief Ostream for writing text to files in HTML format.
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license LGPLv3
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include <ostream_color_log/html_ofstream.h>

#include "HtmlEscape.h"

///////////////////////////////////////////////////////////////////////////////

namespace ostream_color_log {

	html_ofstream::html_ofstream() {
		init(&_filebuf);
	}
	html_ofstream::html_ofstream(const char* name, ios_base::openmode mode,
			ostream_colors foreground, ostream_colors background,
			ostream_attributes attribute) {
		init(&_filebuf);
		open(name, mode);
	}

	// TODO Add constructor and open with std::string as file name.

	void html_ofstream::open(
			const char* name,
			ios_base::openmode mode,
			ostream_colors foreground,
			ostream_colors background,
			ostream_attributes attribute){
		html_filebuf* ret = _filebuf.open(name,
				mode | ios_base::out,
				foreground,
				background,
				attribute);
		if(!ret){
			setstate(ios_base::failbit);
		}else{
			clear();
		}
	}


	void html_ofstream::close(){
		if(!_filebuf.close()){
			setstate(ios_base::failbit);
		}
	}

	html_ofstream& operator<<(html_ofstream& hofs,
			ostream_colors foreground){
		html_filebuf& fb = *hofs.rdbuf();
		fb.pubsync();
		fb.setForeground(foreground);
		fb.directWrite("</p><p style = \"");
		fb.writeAttributeAndColors();
		fb.directWrite("\">");
		return hofs;
	}

	html_ofstream& operator<<(html_ofstream& hofs,
			ostream_attributes attribute){
		html_filebuf& fb = *hofs.rdbuf();
		fb.pubsync();
		fb.setAttribute(attribute);
		fb.directWrite("</p><p style = \"");
		fb.writeAttributeAndColors();
		fb.directWrite("\">");
		return hofs;
	}

	html_ofstream& operator<<(html_ofstream& hofs,
			_OstreamAttributeAndColorFormat format){
		html_filebuf& fb = *hofs.rdbuf();
		fb.pubsync();
		fb.setForeground(format.foreground);
		fb.setBackground(format.background);
		fb.setAttribute(format.attribute);
		fb.directWrite("</p><p style = \"");
		fb.writeAttributeAndColors();
		fb.directWrite("\">");
		return hofs;
	}



} // namespace ostream_color_log{

///////////////////////////////////////////////////////////////////////////////
