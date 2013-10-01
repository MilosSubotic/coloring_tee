/**
 * @file html_ofstream.cpp
 * @date May 4, 2012
 *
 * @brief Ostream for writing text to files in HTML format.
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license GPLv3
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include <ostream_color_log/html_ofstream.h>

#include "HtmlEscape.h"

///////////////////////////////////////////////////////////////////////////////

namespace ostream_color_log {

	html_ofstream& flush(html_ofstream& hofs){
		*hofs.thisOfstream() << flush;
		return hofs;
	}

	html_ofstream& endl(html_ofstream& hofs){
		*hofs.thisOfstream() << endl;
		return hofs;
	}

	html_ofstream& ends(html_ofstream& hofs){
		*hofs.thisOfstream() << '\0';
		return hofs;
	}

	html_ofstream& operator<<(html_ofstream& hofs,
			ostream_colors foreground){
		hofs.setForeground(foreground);
		*hofs.thisOfstream() << "</p><p style = \"";
		hofs.writeAttributeAndColors();
		*hofs.thisOfstream() << "\">";
		return hofs;
	}

	html_ofstream& operator<<(html_ofstream& hofs,
			ostream_attributes attribute){
		hofs.setAttribute(attribute);
		*hofs.thisOfstream() << "</p><p style = \"";
		hofs.writeAttributeAndColors();
		*hofs.thisOfstream() << "\">";
		return hofs;
	}

	html_ofstream& operator<<(html_ofstream& hofs,
			_OstreamAttributeAndColorFormat format){
		hofs.setForeground(format.foreground);
		hofs.setBackground(format.background);
		hofs.setAttribute(format.attribute);
		*hofs.thisOfstream() << "</p><p style = \"";
		hofs.writeAttributeAndColors();
		*hofs.thisOfstream() << "\">";
		return hofs;
	}

	const char* html_ofstream::makeColorString(ostream_colors color){
		switch(color){
		case black:
			return "#000";
		case red:
			return "#f00";
		case green:
			return "#0f0";
		case yellow:
			return "#ff0";
		case blue:
			return "#00f";
		case magenta:
			return "#f0f";
		case cyan:
			return "#0ff";
		case white:
		default:
			return "#fff";
		}
	}

	void html_ofstream::htmlescapeTextArea(const std::string& text){
		*thisOfstream() << HtmlEscape::escapeTextArea(text);
	}

	void html_ofstream::writeAttributeAndColors(){
		*thisOfstream() << _foreground << _background << _attributes;
	}

	void html_ofstream::setForeground(ostream_colors foreground){
		_foreground = std::string("color: ") + makeColorString(foreground)
				+ "; ";
	}

	void html_ofstream::setBackground(ostream_colors background){
		_background = std::string("background-color: ")
				+ makeColorString(background) + "; ";
	}

	void html_ofstream::setAttribute(ostream_attributes attribute){
		switch(attribute){
		case reset:
			_attributes = "";
			_foreground = "";
			_background = "";
			break;
		case bold:
			_attributes += "font-weight: bold; ";
			break;
		case dim:
			break;
		case underline_v2:
			_attributes += "text-decoration: underline; ";
			break;
		case underline:
			_attributes += "text-decoration: underline; ";
			break;
		case blink:
			_attributes += "text-decoration:blink; ";
			break;
		case nothing:
			break;
		case reverse:
			break;
		case hidden:
			_attributes += "visibility: hidden; ";
			break;
		}
	}

	html_ofstream::html_ofstream(const char* name, ios_base::openmode mode,
			_OstreamAttributeAndColorFormat format)
			: std::ofstream(name, mode){
		init(name, format);
	}

	html_ofstream::html_ofstream(const char* name, ios_base::openmode mode,
			ostream_colors foreground, ostream_colors background,
			ostream_attributes attribute)
			: std::ofstream(name, mode){

		init(name, color(foreground, background, attribute));
	}

	void html_ofstream::close(){
		*thisOfstream() << "  </p>\n"
				" </body>\n"
				"</html>\n" << std::endl;
		thisOfstream()->close();
	}

	void html_ofstream::init(const char* name,
			_OstreamAttributeAndColorFormat format){
		setForeground(format.foreground);
		setBackground(format.background);
		setAttribute(format.attribute);
		*thisOfstream() << "<!DOCTYPE html>\n"
				"<html>\n"
				" <head>\n"
				" <title>" << name << "</title>\n"
				"  <style>\n"
				"   p{ white-space: pre-wrap; font-family: monospace;\n"
				"    margin: 0; padding:0;\n"
				"    ";
		writeAttributeAndColors();
		*thisOfstream() << " }\n"
				"  </style>\n"
				" </head>\n"
				" <body bgcolor=\"black\">\n"
				"  <p>";

	}

} // namespace ostream_color_log{

///////////////////////////////////////////////////////////////////////////////
