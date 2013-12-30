/**
 * @file HtmlEscape.h
 * @date May 4, 2012
 *
 * @brief Escape text to HTML style.
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license LGPLv3
 *
 * This code is rewriten from Java code. Here is file comment of
 * original author:
 *
 * HtmlEscape in Java, which is compatible with utf-8
 * @author Ulrich Jensen, http://www.htmlescape.net
 * Feel free to get inspired, use or steal this code and use it in your
 * own projects.
 * License:
 * You have the right to use this code in your own project or publish it
 * on your own website.
 * If you are going to use this code, please include the author lines.
 * Use this code at your own risk. The author does not warrent or assume any
 * legal liability or responsibility for the accuracy, completeness or usefullness of
 * this program code.
 *
 */

#include "HtmlEscape.h"

#include <string>
#include <cstdio>

#include <iostream>

namespace HtmlEscape{


#define ESCAPE_TAGS case '<':out.append("&lt;"); break; \
            case '>':out.append("&gt;"); break; \
            case '"':out.append("&quot;"); break;

#define ESCAPE_BR case '\n': out.append("<br/>"); break; \
            case '\r': break;

#define ESCAPE_SPECIAL case 38:out.append("&amp;"); break; \
			case 198:out.append("&AElig;"); break; \
			case 193:out.append("&Aacute;"); break; \
			case 194:out.append("&Acirc;"); break; \
			case 192:out.append("&Agrave;"); break; \
			case 197:out.append("&Aring;"); break; \
			case 195:out.append("&Atilde;"); break; \
			case 196:out.append("&Auml;"); break; \
			case 199:out.append("&Ccedil;"); break; \
			case 208:out.append("&ETH;"); break; \
			case 201:out.append("&Eacute;"); break; \
			case 202:out.append("&Ecirc;"); break; \
			case 200:out.append("&Egrave;"); break; \
			case 203:out.append("&Euml;"); break; \
			case 205:out.append("&Iacute;"); break; \
			case 206:out.append("&Icirc;"); break; \
			case 204:out.append("&Igrave;"); break; \
			case 207:out.append("&Iuml;"); break; \
			case 209:out.append("&Ntilde;"); break; \
			case 211:out.append("&Oacute;"); break; \
			case 212:out.append("&Ocirc;"); break; \
			case 210:out.append("&Ograve;"); break; \
			case 216:out.append("&Oslash;"); break; \
			case 213:out.append("&Otilde;"); break; \
			case 214:out.append("&Ouml;"); break; \
			case 222:out.append("&THORN;"); break; \
			case 218:out.append("&Uacute;"); break; \
			case 219:out.append("&Ucirc;"); break; \
			case 217:out.append("&Ugrave;"); break; \
			case 220:out.append("&Uuml;"); break; \
			case 221:out.append("&Yacute;"); break; \
			case 225:out.append("&aacute;"); break; \
			case 226:out.append("&acirc;"); break; \
			case 230:out.append("&aelig;"); break; \
			case 224:out.append("&agrave;"); break; \
			case 229:out.append("&aring;"); break; \
			case 227:out.append("&atilde;"); break; \
			case 228:out.append("&auml;"); break; \
			case 231:out.append("&ccedil;"); break; \
			case 233:out.append("&eacute;"); break; \
			case 234:out.append("&ecirc;"); break; \
			case 232:out.append("&egrave;"); break; \
			case 240:out.append("&eth;"); break; \
			case 235:out.append("&euml;"); break; \
			case 237:out.append("&iacute;"); break; \
			case 238:out.append("&icirc;"); break; \
			case 236:out.append("&igrave;"); break; \
			case 239:out.append("&iuml;"); break; \
			case 241:out.append("&ntilde;"); break; \
			case 243:out.append("&oacute;"); break; \
			case 244:out.append("&ocirc;"); break; \
			case 242:out.append("&ograve;"); break; \
			case 248:out.append("&oslash;"); break; \
			case 245:out.append("&otilde;"); break; \
			case 246:out.append("&ouml;"); break; \
			case 223:out.append("&szlig;"); break; \
			case 254:out.append("&thorn;"); break; \
			case 250:out.append("&uacute;"); break; \
			case 251:out.append("&ucirc;"); break; \
			case 249:out.append("&ugrave;"); break; \
			case 252:out.append("&uuml;"); break; \
			case 253:out.append("&yacute;"); break; \
			case 255:out.append("&yuml;"); break; \
			case 162:out.append("&cent;"); break; \



    /**
     * Function for HTML escaping a string, for use in a textarea.
     * @param original The String to escape
     * @return The escaped String
     */
    std::string escapeTextArea(std::string original){
        using namespace std;

        string out;

        string::iterator iter = original.begin(), end = original.end();
        for(; iter != end ; iter++){
			int c = *iter;
            switch( c ){
            ESCAPE_TAGS
            ESCAPE_SPECIAL
            default:
                if( c > 127 ){
                    char x[9];
                    sprintf(x, "&#x%4x;", c);
                }else
                  out.append(1, *iter);
                break;
            }
        }

        return out;
    }

    /**
     * Normal escape function, for Html escaping Strings
     * @param original The original String
     * @return The escape String
     */
    std::string escape(std::string original){
        using namespace std;

        string out;

        string::iterator iter = original.begin(), end = original.end();
        for(; iter != end ; iter++){
			int c = *iter;
            switch( c ){
            ESCAPE_TAGS
            ESCAPE_BR
            ESCAPE_SPECIAL
            default:
                if( c > 127 ){
                    char x[9];
                    sprintf(x, "&#x%4x;", c);
                }else
                  out.append(1, *iter);
                break;
            }
        }

        return out;
    }

    std::string escapeTags(std::string original){
        using namespace std;

        string out;

        string::iterator iter = original.begin(), end = original.end();
        for(; iter != end ; iter++){
			int c = *iter;
            switch( c ){
            ESCAPE_TAGS
            default:out.append(1, c); break;
            }
        }

        return out;
    }

    std::string escapeBr(std::string original){
        using namespace std;

        string out;

        string::iterator iter = original.begin(), end = original.end();
        for(; iter != end ; iter++){
			int c = *iter;
            switch( c ){
            ESCAPE_BR
            default:out.append(1, c); break;
            }
        }

        return out;
    }

    std::string escapeSpecial(std::string original){
        using namespace std;

        string out;

        string::iterator iter = original.begin(), end = original.end();
        for(; iter != end ; iter++){
			int c = *iter;
            switch( c ){
            ESCAPE_SPECIAL
            default:
                if( c > 127 ){
                    char x[9];
                    sprintf(x, "&#x%4x;", c);
                }else
                  out.append(1, c);
                break;
            }
        }

        return out;
    }

} //namespace HtmlEscape{

