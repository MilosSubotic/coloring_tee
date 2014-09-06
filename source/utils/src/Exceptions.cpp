/**
 * @file Exceptions.cpp
 * @date Jan 8, 2014
 *
 * @author Milos Subotic <milos.subotic.sm@gmail.com>
 * @license LGPLv3
 *
 * @brief Better exception classes.
 *
 * @version 1.0
 * Changelog:
 * 1.0 - Initial version.
 *
 */
///////////////////////////////////////////////////////////////////////////////

#include "Exceptions.h"

///////////////////////////////////////////////////////////////////////////////

Exception::~Exception() noexcept{
}

const char* Exception::what() const noexcept {
	if(_name && _name[0]){
		_what = _name;
		_what += ":\n";
		_what += _oss.str();
	} else{
		_what = _oss.str();
	}
	return _what.c_str();
}

Exception::Exception(const Exception& e)
	: _name(e._name) {
	_oss << e._oss.str();
}

Exception& Exception::operator=(const Exception& e){
	if(this != &e){
		_name = e._name;
		_oss << e._oss.str();
	}
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
