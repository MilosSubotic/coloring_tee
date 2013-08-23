/**
 * @file format.cpp
 * @date Feb 7, 2012
 *
 * @brief Format string with variable argument list.
 * @version 3.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license GPLv3
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include "format.h"
#include <malloc.h>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <stdarg.h>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////

static void* xmalloc(size_t size) throw(std::runtime_error){
	register void *value = malloc(size);
	if(!value){
		throw std::runtime_error("Virtual memory exhausted!");
	}
	return value;
}
static void* xrealloc(void *ptr, size_t size) throw(std::runtime_error){
	register void *value = realloc(ptr, size);
	if(!value){
		throw std::runtime_error("Virtual memory exhausted!");
	}
	return value;
}

/**
 * Format string with variable argument list.
 * @param printf like format c string
 * @return formated string
 */
std::string format(const char* format, ...) throw(std::runtime_error){

	// Good hint for start size.
	size_t size = strlen(format) + 1;
	// Guess we need no more than 64 chars of space.
	char* buffer = (char*)xmalloc(size);

	va_list argp;

	// Try to print in the allocated space.
	va_start(argp, format);
	int nchars = vsnprintf(buffer, size, format, argp);
	va_end(argp);

	if(nchars >= size){
		// Reallocate buffer now that we know
		// how much space is needed.
		size = nchars + 1;
		buffer = (char*)xrealloc(buffer, size);

		// Try again.
		va_start(argp, format);
		vsnprintf(buffer, size, format, argp);
		va_end(argp);
	}

	std::string retVal(buffer);

	free(buffer);

	return retVal;
}
