/**
 * @file path.cpp
 * @date May 28, 2013
 *
 * @brief
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license LGPLv3
 *
 */

///////////////////////////////////////////////////////////////////////////////
#include "os/path.h"

#include <algorithm>

///////////////////////////////////////////////////////////////////////////////

namespace os {

	namespace path {

		std::string join(const std::string& s0, const std::string& s1) {
			std::string r(s0);
			if(!r.empty()){
				// Last char is not '/'?
				if(*(r.end()-1) != '/'){
					// Add it.
					r += '/';
				}
			}
			r += s1;
			return r;
		}

		static bool doubleSlash(char a, char b){
			return a == '/' && b == '/';
		}

		std::string normpath(const std::string& s) {
			std::string r(s);

			// A\B to A/B
			replace(r.begin(), r.end(), '\\', '/');

			// A//B to A/B
			r.erase(
				std::unique(r.begin(), r.end(), doubleSlash), 
				r.end()
			);

			// A/B/ to A/B
			if(!r.empty()){
				// Last char is '/'?
				if(*(r.end()-1) == '/'){
					// Erase it.
					r.erase(r.size() - 1);
				}
			}

			// TODO  A/foo/../B to A/B
			return r;
		}

	} // namespace path

} // namespace os

///////////////////////////////////////////////////////////////////////////////
