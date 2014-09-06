/**
 * @file type_traits.h
 * @date May 5, 2014
 *
 * @author Milos Subotic <milos.subotic.sm@gmail.com>
 * @license LGPLv3
 *
 * @brief Additional type traits.
 *
 * @version 1.0
 * Changelog:
 * 1.0 - Initial version.
 *
 */

#ifndef TYPE_TRAITS_H_
#define TYPE_TRAITS_H_

///////////////////////////////////////////////////////////////////////////////

#include <type_traits>

///////////////////////////////////////////////////////////////////////////////

namespace stl_extensions {

	template<typename T, typename U>
	struct is_related :
		std::is_same<
			typename std::remove_cv<
				typename std::remove_reference<T>::type
			>::type,
			typename std::remove_cv<
				typename std::remove_reference<U>::type
			>::type
		> {

	};
	template<typename T, typename U, typename ... Us>
	struct enable_if_unrelated : std::enable_if<!is_related<T, U>::value> {
	};

} // namespace stl_extensions

///////////////////////////////////////////////////////////////////////////////

#endif // TYPE_TRAITS_H_
