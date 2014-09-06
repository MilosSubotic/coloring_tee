/**
 * @file html_filebuf.cpp
 * @date Dec 30, 2013
 *
 * @author Milos Subotic <milos.subotic.sm@gmail.com>
 * @license LGPLv3
 *
 * @brief
 *
 * @version 1.0
 * Changelog:
 * 1.0 - Initial version.
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include "ostream_color_log/html_filebuf.h"

#include <cstring>
#include <limits>

///////////////////////////////////////////////////////////////////////////////

namespace ostream_color_log {


#define writeToBuffer(s) _filebuf.sputn(s, sizeof(s)-1);

#define ESCAPE_TAGS \
	case '<': writeToBuffer("&lt;"); break; \
    case '>': writeToBuffer("&gt;"); break; \
    case '"': writeToBuffer("&quot;"); break;

#define ESCAPE_BR \
	case '\n': writeToBuffer("<br/>"); break; \
    case '\r': break;

#define ESCAPE_SPECIAL \
	case 38: writeToBuffer("&amp;"); break; \
	case 198: writeToBuffer("&AElig;"); break; \
	case 193: writeToBuffer("&Aacute;"); break; \
	case 194: writeToBuffer("&Acirc;"); break; \
	case 192: writeToBuffer("&Agrave;"); break; \
	case 197: writeToBuffer("&Aring;"); break; \
	case 195: writeToBuffer("&Atilde;"); break; \
	case 196: writeToBuffer("&Auml;"); break; \
	case 199: writeToBuffer("&Ccedil;"); break; \
	case 208: writeToBuffer("&ETH;"); break; \
	case 201: writeToBuffer("&Eacute;"); break; \
	case 202: writeToBuffer("&Ecirc;"); break; \
	case 200: writeToBuffer("&Egrave;"); break; \
	case 203: writeToBuffer("&Euml;"); break; \
	case 205: writeToBuffer("&Iacute;"); break; \
	case 206: writeToBuffer("&Icirc;"); break; \
	case 204: writeToBuffer("&Igrave;"); break; \
	case 207: writeToBuffer("&Iuml;"); break; \
	case 209: writeToBuffer("&Ntilde;"); break; \
	case 211: writeToBuffer("&Oacute;"); break; \
	case 212: writeToBuffer("&Ocirc;"); break; \
	case 210: writeToBuffer("&Ograve;"); break; \
	case 216: writeToBuffer("&Oslash;"); break; \
	case 213: writeToBuffer("&Otilde;"); break; \
	case 214: writeToBuffer("&Ouml;"); break; \
	case 222: writeToBuffer("&THORN;"); break; \
	case 218: writeToBuffer("&Uacute;"); break; \
	case 219: writeToBuffer("&Ucirc;"); break; \
	case 217: writeToBuffer("&Ugrave;"); break; \
	case 220: writeToBuffer("&Uuml;"); break; \
	case 221: writeToBuffer("&Yacute;"); break; \
	case 225: writeToBuffer("&aacute;"); break; \
	case 226: writeToBuffer("&acirc;"); break; \
	case 230: writeToBuffer("&aelig;"); break; \
	case 224: writeToBuffer("&agrave;"); break; \
	case 229: writeToBuffer("&aring;"); break; \
	case 227: writeToBuffer("&atilde;"); break; \
	case 228: writeToBuffer("&auml;"); break; \
	case 231: writeToBuffer("&ccedil;"); break; \
	case 233: writeToBuffer("&eacute;"); break; \
	case 234: writeToBuffer("&ecirc;"); break; \
	case 232: writeToBuffer("&egrave;"); break; \
	case 240: writeToBuffer("&eth;"); break; \
	case 235: writeToBuffer("&euml;"); break; \
	case 237: writeToBuffer("&iacute;"); break; \
	case 238: writeToBuffer("&icirc;"); break; \
	case 236: writeToBuffer("&igrave;"); break; \
	case 239: writeToBuffer("&iuml;"); break; \
	case 241: writeToBuffer("&ntilde;"); break; \
	case 243: writeToBuffer("&oacute;"); break; \
	case 244: writeToBuffer("&ocirc;"); break; \
	case 242: writeToBuffer("&ograve;"); break; \
	case 248: writeToBuffer("&oslash;"); break; \
	case 245: writeToBuffer("&otilde;"); break; \
	case 246: writeToBuffer("&ouml;"); break; \
	case 223: writeToBuffer("&szlig;"); break; \
	case 254: writeToBuffer("&thorn;"); break; \
	case 250: writeToBuffer("&uacute;"); break; \
	case 251: writeToBuffer("&ucirc;"); break; \
	case 249: writeToBuffer("&ugrave;"); break; \
	case 252: writeToBuffer("&uuml;"); break; \
	case 253: writeToBuffer("&yacute;"); break; \
	case 255: writeToBuffer("&yuml;"); break; \
	case 162: writeToBuffer("&cent;"); break; \



	html_filebuf::html_filebuf() {
	}

	html_filebuf::~html_filebuf() {
		close();
	}

	html_filebuf* html_filebuf::open(
			const char* name,
			std::ios_base::openmode mode,
			ostream_colors foreground,
			ostream_colors background,
			ostream_attributes attribute){
		std::filebuf* ret =	_filebuf.open(name, mode);
		if(ret){
			setForeground(foreground);
			setBackground(background);
			setAttribute(attribute);
			directWrite(
					"<!DOCTYPE html>\n"
					"<html>\n"
					" <head>\n"
					" <title>");
			directWrite(name);
			directWrite(
					"</title>\n"
					"  <style>\n"
					"   p{ white-space: pre-wrap; font-family: monospace;\n"
					"    margin: 0; padding:0;\n"
					"    ");
			writeAttributeAndColors();
			directWrite(
					" }\n"
					"  </style>\n"
					" </head>\n"
					" <body bgcolor=\"black\">\n"
					"  <p>");
			return this;
		}else{
			return 0;
		}
	}

	html_filebuf* html_filebuf::close(){
		if(!is_open()){
			return 0;
		}

		directWrite("  </p>\n"
				" </body>\n"
				"</html>\n");

		std::filebuf* ret =	_filebuf.close();
		if(ret){
			return this;
		}else{
			return 0;
		}
	}

	/**
	 *  @brief  Synchronizes the buffer arrays with the controlled sequences.
	 *  @return  -1 on failure.
	 *
	 *  Each derived class provides its own appropriate behavior,
	 *  including the definition of @a failure.
	 *  @note  Base class version does nothing, returns zero.
	 */
	int html_filebuf::sync() {
		return _filebuf.pubsync();
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
	std::streamsize html_filebuf::xsputn(const char* s, std::streamsize n) {
		// TODO Optimize.
		// Escaping text.
		std::streamsize i;
        for(i = 0; i < n && *s; i++, s++){
			// TODO Check if all is written in buffer.
			int c = *s;
            switch(c){
            ESCAPE_TAGS
            ESCAPE_SPECIAL
            default:
                if( c > 127 ){
                    char buffer[9];
                    int stored = snprintf(buffer, 9, "&#x%4x;", c);
                    _filebuf.sputn(buffer, stored);
                }else{
                	 _filebuf.sputc(c);
                }
                break;
            }
        }
		return i;
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
	std::streambuf::int_type html_filebuf::overflow(
			std::streambuf::int_type c) {
		if(c == traits_type::eof())
			return traits_type::eof();
		else{
			char_type ch = traits_type::to_char_type(c);
			return xsputn(&ch, 1) == 1 ? c : traits_type::eof();
		}
	}

	std::streamsize html_filebuf::directWrite(const char* s){
		return _filebuf.sputn(s, strlen(s));
	}

	std::streamsize html_filebuf::directWrite(const std::string& s){
		return _filebuf.sputn(s.c_str(), s.size());
	}

	const char* html_filebuf::makeColorString(ostream_colors color) {
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

	void html_filebuf::writeAttributeAndColors() {
		directWrite(_foreground);
		directWrite(_background);
		directWrite(_attributes);
	}

	void html_filebuf::setForeground(ostream_colors foreground) {
		_foreground = std::string("color: ") + makeColorString(foreground)
				+ "; ";
	}

	void html_filebuf::setBackground(ostream_colors background) {
		_background = std::string("background-color: ")
				+ makeColorString(background) + "; ";
	}

	void html_filebuf::setAttribute(ostream_attributes attribute) {
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

} // namespace ostream_color_log

///////////////////////////////////////////////////////////////////////////////
