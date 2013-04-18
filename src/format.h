/**
 * @file format.h
 * @date Feb 7, 2012
 *
 * @brief
 * @version 2.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license GPLv3
 *
 */

#ifndef FORMAT_H_
#define FORMAT_H_

///////////////////////////////////////////////////////////////////////////////

#include <string>

///////////////////////////////////////////////////////////////////////////////

/**
 * Format string with variable argument list.
 * @param printf like format c string
 * @return formated string
 */
std::string format(const char *format, ...);

///////////////////////////////////////////////////////////////////////////////

#endif /* FORMAT_H_ */
