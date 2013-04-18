/**
 * @file LuaConfig.h
 * @date Feb 7, 2012
 *
 * @brief
 * @version 2.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license GPLv3
 *
 */

#ifndef LUACONFIG_H_
#define LUACONFIG_H_

///////////////////////////////////////////////////////////////////////////////

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <string>
#include <cstdlib>
#include <stdexcept>

#include "CommonMacros.h"

#include "format.h"

///////////////////////////////////////////////////////////////////////////////

class LuaConfigError: public std::runtime_error{
public:

	/** Takes a character std::string describing the error. */
	explicit LuaConfigError(const std::string& message)
			: std::runtime_error(message){
	}
	/** Accumulate earlier errors */
	explicit LuaConfigError(const LuaConfigError& e, const std::string& message)
			: std::runtime_error(message + "\n" + e.what()){
	}

};

/**
 * C++ wrapper for Lua configuration file reader.
 */
class LuaConfig{
public:
	lua_State* L;

public:
	/// For later deferred init.
	LuaConfig()
			: L(NULL){
	}

	/**
	 * Open Lua  config file.
	 * @param fileName of Lua config file.
	 * @throw if there is some error.
	 */
	LuaConfig(const char* fileName) throw(LuaConfigError);

	~LuaConfig() noexcept{
		close();
	}

	/**
	 * Deferred init, open Lua config file.
	 * @param fileName of Lua config file.
	 */
	LuaConfig& open(const char* fileName) throw(LuaConfigError);

	/**
	 * Closing Lua config file reader.
	 */
	void close() noexcept{
		if(L){
			lua_close(L);
		}
		L = NULL;
	}

	/**
	 * Pop something from Lua stack.
	 */
	LuaConfig& pop() noexcept;

	/**
	 * Read global table on top of Lua stack.
	 * @param table
	 */
	LuaConfig& getGlobalTable(const char* table) throw(LuaConfigError);

	/**
	 * Read std::string from table on top of Lua stack.
	 * @param key for access a table.
	 * @return read std::string.
	 */
	std::string getFieldString(const char *key) throw(LuaConfigError);
	std::string getFieldString(int key) throw(LuaConfigError);

	/**
	 * Read int from table on top of Lua stack.
	 * @param key for access a table.
	 * @return read integer.
	 */
	int getFieldInt(const char *key) throw(LuaConfigError);
	/**
	 * Read double from table on top of Lua stack.
	 * @param key for access a table.
	 * @return read double number.
	 */
	double getFieldDouble(const char *key) throw(LuaConfigError);
	double getFieldDouble(int key) throw(LuaConfigError);

	/**
	 * Read new table to top of stack from table on top of stack.
	 * @param key for access a table.
	 */
	LuaConfig& getFieldTable(const char *key) throw(LuaConfigError);
	LuaConfig& getFieldTable(int key) throw(LuaConfigError);

	// For iterating tables.
	void iterationInit() noexcept;
	bool iterationCondition() noexcept;
	void iterationIncrement() noexcept;
	std::string getKeyAsString() throw(LuaConfigError);
};

/**
 * This class unwind Lua stack.
 * It should be written just below try{
 * and constructor use LuaConfig or lua_State to
 * get Lua stack top index and destructor return
 * old index.
 */
class LuaConfigUnwinder{
protected:
	lua_State* luaState;
	int luaStackTopIndex;
public:
	LuaConfigUnwinder(lua_State* L) noexcept{
		luaState = L;
		luaStackTopIndex = lua_gettop(L);
	}

	LuaConfigUnwinder(const LuaConfig& lc) noexcept{
		luaState = lc.L;
		luaStackTopIndex = lua_gettop(luaState);
	}

	~LuaConfigUnwinder() noexcept{
		lua_settop(luaState, luaStackTopIndex);
	}

};

///////////////////////////////////////////////////////////////////////////////

#endif // LUACONFIG_H_
