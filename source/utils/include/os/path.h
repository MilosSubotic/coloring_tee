/**
 * @file path.h
 * @date May 28, 2013
 *
 * @brief
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license LGPLv3
 *
 */

#ifndef PATH_H_
#define PATH_H_

///////////////////////////////////////////////////////////////////////////////

#include <string>

///////////////////////////////////////////////////////////////////////////////

namespace os {

	namespace path {

		std::string join(const std::string& s0, const std::string& s1);
		std::string normpath(const std::string& s);
		std::string dirname(const std::string& s);

	} // namespace path

} // namespace os

///////////////////////////////////////////////////////////////////////////////

#endif // PATH_H_
