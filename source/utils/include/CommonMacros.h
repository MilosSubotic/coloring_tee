/**
 * @file CommonMacros.h
 * @date Mar 8, 2013
 *
 * @brief
 * @version 2.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license LGPLv3
 *
 */

#ifndef COMMONMACROS_H_
#define COMMONMACROS_H_

///////////////////////////////////////////////////////////////////////////////

#define GCC_VERSION_LOWER_THAN(major, minor) \
	(__GNUC__ < (major) || __GNUC_MINOR__ < (minor))

#if !defined(__GXX_EXPERIMENTAL_CXX0X__) || GCC_VERSION_LOWER_THAN(4, 6)

#define constexpr

#define noexcept throw()

#define nullptr NULL

namespace std {
	typedef void* nullptr_t;
}

#endif

///////////////////////////////////////////////////////////////////////////////

#define foreach(element, collection) \
	for(auto ___iter___ = collection.begin(), ___end___ = collection.end(); \
			___iter___ != ___end___; ___iter___++) \
		for(element = *___iter___;; __extension__ ({ break; }))

#define foreach_in_map(key, value, collection) \
	for(auto ___iter___ = collection.begin(), ___end___ = collection.end(); \
			___iter___ != ___end___; ___iter___++) \
		for(key = ___iter___->first;; __extension__ ({ break; })) \
			for(value = ___iter___->second;; __extension__ ({ break; }))

#define if_in_map(key, map, iter) \
		auto iter =	map.find(key); \
		if(iter != map.end())

#define exists_in_string(what, in) \
	( (in).find(what) != std::string::npos )

///////////////////////////////////////////////////////////////////////////////

#endif // COMMONMACROS_H_
