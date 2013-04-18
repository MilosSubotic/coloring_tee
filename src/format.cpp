/**
 * @file format.cpp
 * @date Feb 7, 2012
 *
 * @brief
 * @version 2.0
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

///////////////////////////////////////////////////////////////////////////////

static void* xmalloc(size_t size){
	register void *value = malloc(size);
	if(value == 0){
		fprintf(stderr, "Virtual memory exhausted!");
		exit(-1);
	}
	return value;
}
static void* xrealloc(void *ptr, size_t size){
	register void *value = realloc(ptr, size);
	if(value == 0){
		fprintf(stderr, "Virtual memory exhausted!");
		exit(-1);
	}
	return value;
}

/**
 * Format string with variable argument list.
 * @param fmt
 * @return
 */
std::string format(const char *format, ...){
	va_list argp;
	va_start(argp, format);

	int size = 64;
	// Guess we need no more than 64 chars of space.
	char *buffer = (char *) xmalloc(size);

	if(buffer == NULL){
		return std::string(); //return string("No more memory!");
	}

	// Try to print in the allocated space.
	int nchars = vsnprintf(buffer, size, format, argp);

	if(nchars >= size){
		// Reallocate buffer now that we know
		// how much space is needed.
		size = nchars + 1;
		buffer = (char *) xrealloc(buffer, size);
		if(buffer != NULL)
			/* Try again. */
			vsnprintf(buffer, size, format, argp);
	}
	va_end(argp);

	return std::string(buffer);

}
