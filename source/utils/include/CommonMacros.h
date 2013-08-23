/**
 * @file CommonMacros.h
 * @date Mar 8, 2013
 *
 * @brief
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license GPLv3
 *
 */

#ifndef COMMONMACROS_H_
#define COMMONMACROS_H_

///////////////////////////////////////////////////////////////////////////////

#define GCC_VERSION_LOWER_THAN(major, minor) \
	(__GNUC__ < (major) || __GNUC_MINOR__ < (minor))

#if !defined(__GXX_EXPERIMENTAL_CXX0X__)

#define constexpr

namespace std {
	enum nullptr_t { nullptr };
}

#endif

#if !defined(__GXX_EXPERIMENTAL_CXX0X__) || GCC_VERSION_LOWER_THAN(4, 6)

#define noexcept throw()

#endif

///////////////////////////////////////////////////////////////////////////////

#endif /* COMMONMACROS_H_ */
