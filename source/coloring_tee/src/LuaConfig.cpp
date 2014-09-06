/**
 * @file LuaConfig.cpp
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

///////////////////////////////////////////////////////////////////////////////

#include "LuaConfig.h"

#include <cassert>

///////////////////////////////////////////////////////////////////////////////

LuaConfig& LuaConfig::open(const char* fileName) {
	if(L){
		lua_close(L);
	}

	L = luaL_newstate();
	luaL_openlibs(L);
	if(luaL_loadfile(L, fileName) || lua_pcall(L, 0, 0, 0)){
		throw LuaConfigError() << EXCEPTION_FROM_HERE
				<< "Cannot run config file \"" << fileName
				<< "\". Error message:\n"
				<< lua_tostring(L, -1) << endl;
	}
	return *this;
}

LuaConfig::LuaConfig(const char* fileName) {
	L = nullptr;
	open(fileName);
}

/**
 * Pop something from Lua stack.
 */
LuaConfig& LuaConfig::pop() noexcept {
	assert(L);
	lua_pop(L, 1);
	return *this;
}

LuaConfig& LuaConfig::getGlobalTable(const char* table) {
	assert(L);
	lua_getglobal(L, table);
	if(!lua_istable(L, -1)){
		throw LuaConfigError() << EXCEPTION_FROM_HERE
				<< '"' << table << "\" is not a table!" << endl;
	}
	return *this;
}

bool LuaConfig::haveField(const char* key){
	assert(L);
	lua_getfield(L, -1, key);
	bool res = !lua_isnoneornil(L, -1);
	lua_pop(L, 1);
	return res;
}

bool LuaConfig::haveField(int key){
	assert(L);
	lua_pushinteger(L, key);
	lua_gettable(L, -2);
	bool res = !lua_isnoneornil(L, -1);
	lua_pop(L, 1);
	return res;
}

std::string LuaConfig::getFieldString(const char *key) {
	assert(L);
	lua_getfield(L, -1, key);
	if(!lua_isstring(L, -1)){
		lua_pop(L, 1);
		throw LuaConfigError() << EXCEPTION_FROM_HERE
				<< "Field \"" << key << "\" is not a string!" << endl;
	}
	std::string str(lua_tostring(L, -1));
	lua_pop(L, 1);
	return str;
}
std::string LuaConfig::getFieldString(int key) {
	assert(L);
	lua_pushinteger(L, key);
	lua_gettable(L, -2);
	if(!lua_isstring(L, -1)){
		lua_pop(L, 1);
		throw LuaConfigError() << EXCEPTION_FROM_HERE
				<< "Field \"" << key << "\" is not a string!" << endl;
	}
	std::string str(lua_tostring(L, -1));
	lua_pop(L, 1);
	return str;
}

int LuaConfig::getFieldInt(const char *key) {
	assert(L);
	lua_getfield(L, -1, key);
	if(!lua_isnumber(L, -1)){
		lua_pop(L, 1);
		throw LuaConfigError() << EXCEPTION_FROM_HERE
				<< "Field \"" << key << "\" is not a int!" << endl;
	}
	int result = (int) lua_tointeger(L, -1);
	lua_pop(L, 1);
	return result;
}

double LuaConfig::getFieldDouble(const char *key) {
	assert(L);
	lua_getfield(L, -1, key);
	if(!lua_isnumber(L, -1)){
		lua_pop(L, 1);
		throw LuaConfigError() << EXCEPTION_FROM_HERE
				<< "Field \"" << key << "\" is not a double!" << endl;
	}
	double result = (double) lua_tonumber(L, -1);
	lua_pop(L, 1);
	return result;
}

double LuaConfig::getFieldDouble(int key) {
	assert(L);
	lua_pushinteger(L, key);
	lua_gettable(L, -2);
	if(!lua_isnumber(L, -1)){
		lua_pop(L, 1);
		throw LuaConfigError() << EXCEPTION_FROM_HERE
				<< "Field \""<< key << "\" is not a double!" << endl;
	}
	double result = (double) lua_tonumber(L, -1);
	lua_pop(L, 1);
	return result;
}

LuaConfig& LuaConfig::getFieldTable(const char *key) {
	assert(L);
	lua_getfield(L, -1, key);
	if(!lua_istable(L, -1)){
		//lua_pop(L, 1);
		throw LuaConfigError() << EXCEPTION_FROM_HERE
				<< "Field \"" << key << "\" is not a table!" << endl;
	}
	return *this;
}

LuaConfig& LuaConfig::getFieldTable(int key) {
	assert(L);
	lua_pushinteger(L, key);
	lua_gettable(L, -2);
	if(!lua_istable(L, -1)){
		//lua_pop(L, 1);
		throw LuaConfigError() << EXCEPTION_FROM_HERE
				<< "Field \"" << key << "\" is not a table!" << endl;
	}
	return *this;
}

bool LuaConfig::haveFieldTable(const char* key) {
	assert(L);
	lua_getfield(L, -1, key);
	bool res = lua_istable(L, -1);
	lua_pop(L, 1);
	return res;
}

void LuaConfig::iterationInit() noexcept {
	assert(L);
	lua_pushnil(L);
}

bool LuaConfig::iterationCondition() noexcept {
	assert(L);
	return lua_next(L, -2);
}

void LuaConfig::iterationIncrement() noexcept {
	assert(L);
	lua_pop(L, 1);
}

std::string LuaConfig::getKeyAsString() {
	assert(L);
	if(!lua_isstring(L, -2)){
		throw LuaConfigError() << EXCEPTION_FROM_HERE
				<< "Key is not a string!" << endl;
	}
	lua_pushvalue(L, -2); // Copy to top.
	std::string result(lua_tostring(L, -1));
	lua_pop(L, 1);
	return result;
}

///////////////////////////////////////////////////////////////////////////////

