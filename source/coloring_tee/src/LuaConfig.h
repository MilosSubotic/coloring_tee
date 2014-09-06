/**
 * @file LuaConfig.h
 * @date Feb 7, 2012
 *
 * @author Milos Subotic <milos.subotic.sm@gmail.com>
 * @license LGPLv3
 *
 * @brief C++ helper classes for working with Lua config files.
 *
 * @version 3.1
 * Changelog:
 * 1.0 - Initial version.
 * 2.0 - Modified for using utils package.
 * 3.0 - Modified for using Exception.h.
 * 3.1 - More methods.
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

#include "CommonMacros.h"
#include "Exceptions.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * @class LuaConfigError
 * @brief LuaConfig exception.
 */
class LuaConfigError : public Exception {
public:
	explicit LuaConfigError()
			: Exception("LuaConfigError") {
	}
	explicit LuaConfigError(const std::string& message)
			: Exception("LuaConfigError", message) {
	}
};

///////////////////////////////////////

/**
 * @class LuaConfigError
 * @brief C++ wrapper for Lua configuration file reader.
 */
class LuaConfig {
public:
	lua_State* L;

public:
	/// For later deferred init.
	LuaConfig() noexcept
		: L(nullptr){
	}
	/**
	 * Open Lua  config file.
	 * @param fileName of Lua config file.
	 * @throw if there is some error.
	 */
	LuaConfig(const char* fileName);
	~LuaConfig() noexcept{
		close();
	}

	///////////////////////////////////

public:
	/**
	 * Deferred init, open Lua config file.
	 * @param fileName of Lua config file.
	 */
	LuaConfig& open(const char* fileName);
	/**
	 * Closing Lua config file reader.
	 */
	void close() noexcept{
		if(L){
			lua_close(L);
		}
		L = nullptr;
	}

	/**
	 * Pop something from Lua stack.
	 */
	LuaConfig& pop() noexcept;

	/**
	 * Read global table on top of Lua stack.
	 * @param table
	 */
	LuaConfig& getGlobalTable(const char* table);

	bool haveField(const char* key);
	bool haveField(int key);
	/**
	 * Read std::string from table on top of Lua stack.
	 * @param key for access a table.
	 * @return read std::string.
	 */
	std::string getFieldString(const char* key);
	std::string getFieldString(int key);
	/**
	 * Read int from table on top of Lua stack.
	 * @param key for access a table.
	 * @return read integer.
	 */
	int getFieldInt(const char* key);
	/**
	 * Read double from table on top of Lua stack.
	 * @param key for access a table.
	 * @return read double number.
	 */
	double getFieldDouble(const char *key);
	double getFieldDouble(int key);
	/**
	 * Read new table to top of stack from table on top of stack.
	 * @param key for access a table.
	 */
	LuaConfig& getFieldTable(const char* key);
	LuaConfig& getFieldTable(int key);

	bool haveFieldTable(const char* key);

	// For iterating tables.
	void iterationInit() noexcept;
	bool iterationCondition() noexcept;
	void iterationIncrement() noexcept;
	std::string getKeyAsString();
};

///////////////////////////////////////

/**
 * @class LuaConfigUnwinder
 * @brief This class unwind Lua stack.
 * It should be written just below try{
 * and constructor use LuaConfig or lua_State to
 * get Lua stack top index and destructor return
 * old index.
 */
class LuaConfigUnwinder {
protected:
	lua_State* L;
	int luaStackTopIndex;
public:
	LuaConfigUnwinder(lua_State* L) noexcept {
		this->L = L;
		luaStackTopIndex = lua_gettop(L);
	}

	LuaConfigUnwinder(const LuaConfig& lc) noexcept {
		L = lc.L;
		luaStackTopIndex = lua_gettop(L);
	}

	~LuaConfigUnwinder() noexcept {
		lua_settop(L, luaStackTopIndex);
	}

};

///////////////////////////////////////////////////////////////////////////////

#endif // LUACONFIG_H_
