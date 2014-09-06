/**
 * @file Exceptions.h
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

#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

///////////////////////////////////////////////////////////////////////////////

#include <stdexcept>
#include <sstream>

#include "CommonMacros.h"

///////////////////////////////////////////////////////////////////////////////

#define EXCEPTION_FROM_HERE "  " << __PRETTY_FUNCTION__ << " @ "	\
	<< __LINE__ << ":\n"

// Forward declarations.
class Exception;
inline Exception& endl(Exception& e);

/**
 * @class Exception
 * @brief Exception with ability to concatenate with ostream operator.
 */
class Exception : public std::exception {
	friend inline Exception& endl(Exception& e);
public:
	explicit Exception()
			: _name("Exception") {
	}
	explicit Exception(const std::string& message)
			: _name("Exception"), _oss(message) {
	}

    virtual ~Exception() noexcept;

    ///////////////////////////////////

protected:
    explicit Exception(const char* name)
			: _name(name) {
	}
    explicit Exception(const char* name, const std::string& message)
			: _name(name), _oss(message) {
	}

    ///////////////////////////////////

public:
    Exception(const Exception& e);
    Exception& operator=(const Exception& e);

    ///////////////////////////////////

public:
    /**
     * Concatenate new message on exception message.
     * @param e Exception
     * @param t Data to be concatenated.
     * @return argument e
     */
    template<typename Type>
    Exception& operator<<(const Type& t){
    	_oss << t;
    	return *this;
    }

    /**
     * Ostream function for manipulators.
     * @param pf Manipulator.
     * @return this.
     */
    Exception& operator<<(Exception& (*pf)(Exception&)){
    	return pf(*this);
    }

    ///////////////////////////////////

public:
    /**
     * @return Description of error.
     */
    virtual const char* what() const noexcept;

    ///////////////////////////////////

protected:
    const char* _name;
    std::ostringstream _oss;
    mutable std::string _what;
};

inline Exception& endl(Exception& e){
	e._oss << std::endl;
	return e;
}


class RuntimeError : public Exception {
public:
	explicit RuntimeError()
			: Exception("RuntimeError") {
	}
	explicit RuntimeError(const std::string& message)
			: Exception("RuntimeError", message) {
	}

    ///////////////////////////////////

protected:
    explicit RuntimeError(const char* name)
			: Exception(name) {
	}
    explicit RuntimeError(const char* name, const std::string& message)
			: Exception(name, message) {
	}
};

///////////////////////////////////////////////////////////////////////////////

#endif // EXCEPTIONS_H_
