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

#include <string>

namespace HtmlEscape{
    /**
     * Function for HTML escaping a string, for use in a textarea.
     * @param original The string to escape
     * @return The escaped string
     */
    std::string escapeTextArea(std::string original);

    /**
     * Normal escape function, for Html escaping Strings
     * @param original The original string
     * @return The escape string
     */
    std::string escape(std::string original);

    std::string escapeTags(std::string original);

    std::string escapeBr(std::string original);

    std::string escapeSpecial(std::string original);

} //namespace HtmlEscape{

