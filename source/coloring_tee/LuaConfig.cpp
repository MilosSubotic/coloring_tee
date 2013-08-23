/**
 * @file LuaConfig.cpp
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

#include "LuaConfig.h"

#include <cassert>

///////////////////////////////////////////////////////////////////////////////

#define HANDLE_ERROR(string)                        \
    do{                                             \
        lua_pop(L, 1);                              \
        throw LuaConfigError( format(string, key) );\
    }while(0)

///////////////////////////////////////////////////////////////////////////////

LuaConfig& LuaConfig::open(const char* fileName) throw(LuaConfigError){
	if(L){
		lua_close(L);
	}

	L = luaL_newstate();
	luaL_openlibs(L);
	if(luaL_loadfile(L, fileName) || lua_pcall(L, 0, 0, 0)){
		throw LuaConfigError(
				format("Cannot run config file \"%s\". Error message: %s\n",
						fileName, lua_tostring(L, -1)));
	}
	return *this;
}

LuaConfig::LuaConfig(const char* fileName) throw(LuaConfigError){
	L = NULL;
	open(fileName);
}

/**
 * Pop something from lua stack.
 */
LuaConfig& LuaConfig::pop() noexcept{
	assert(L);
	lua_pop(L, 1);
	return *this;
}

LuaConfig& LuaConfig::getGlobalTable(const char* table)
		throw(LuaConfigError){
	assert(L);
	lua_getglobal(L, table);
	if(!lua_istable(L, -1))
		throw LuaConfigError(format("'%s' is not a table!", table));
	return *this;
}

std::string LuaConfig::getFieldString(const char *key)
		throw(LuaConfigError){
	assert(L);
	lua_getfield(L, -1, key);
	if(!lua_isstring(L, -1)){
		HANDLE_ERROR("Field '%s' is not string!");
	}
	std::string str(lua_tostring(L, -1));
	lua_pop(L, 1);
	return str;
}
std::string LuaConfig::getFieldString(int key) throw(LuaConfigError){
	assert(L);
	lua_pushinteger(L, key);
	lua_gettable(L, -2);
	if(!lua_isstring(L, -1)){
		HANDLE_ERROR("Field %d is not string!");
	}
	std::string str(lua_tostring(L, -1));
	lua_pop(L, 1);
	return str;
}

int LuaConfig::getFieldInt(const char *key) throw(LuaConfigError){
	assert(L);
	lua_getfield(L, -1, key);
	if(!lua_isnumber(L, -1))
		HANDLE_ERROR("Field '%s' is not int!");
	int result = (int) lua_tointeger(L, -1);
	lua_pop(L, 1);
	return result;
}

double LuaConfig::getFieldDouble(const char *key)
		throw(LuaConfigError){
	assert(L);
	lua_getfield(L, -1, key);
	if(!lua_isnumber(L, -1))
		HANDLE_ERROR("Field '%s' is not double!");
	double result = (double) lua_tonumber(L, -1);
	lua_pop(L, 1);
	return result;
}

double LuaConfig::getFieldDouble(int key) throw(LuaConfigError){
	assert(L);
	lua_pushinteger(L, key);
	lua_gettable(L, -2);
	if(!lua_isnumber(L, -1))
		HANDLE_ERROR("Field '%d' is not double!");
	double result = (double) lua_tonumber(L, -1);
	lua_pop(L, 1);
	return result;
}

LuaConfig& LuaConfig::getFieldTable(const char *key)
		throw(LuaConfigError){
	assert(L);
	lua_getfield(L, -1, key);
	if(!lua_istable(L, -1))
		throw LuaConfigError(format("Field '%s' is not table!", key));
	return *this;
}

LuaConfig& LuaConfig::getFieldTable(int key) throw(LuaConfigError){
	assert(L);
	lua_pushinteger(L, key);
	lua_gettable(L, -2);

	if(!lua_istable(L, -1))
		throw LuaConfigError(format("Field '%d' is not table!", key));
	return *this;
}

void LuaConfig::iterationInit() noexcept{
	assert(L);
	lua_pushnil(L);
}

bool LuaConfig::iterationCondition() noexcept{
	assert(L);
	return lua_next(L, -2);
}

void LuaConfig::iterationIncrement() noexcept{
	assert(L);
	lua_pop(L, 1);
}

std::string LuaConfig::getKeyAsString() throw(LuaConfigError){
	assert(L);
	if(!lua_isstring(L, -2)){
		throw LuaConfigError("Key is not a string!");
	}
	return std::string(lua_tostring(L, -2));
}

///////////////////////////////////////////////////////////////////////////////

