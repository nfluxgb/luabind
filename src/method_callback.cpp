#include "luabind/method_callback.hpp"
#include <lua.hpp>
#include <iostream>

namespace luabind {
namespace detail {

double get_number_parameter(lua_State* L, std::size_t n) {
	return lua_tonumber(L, n);
}

std::string get_string_parameter(lua_State* L, std::size_t n) {
	return std::string(lua_tostring(L, n));
}

int set_number_return(lua_State* L, double value) {
	lua_pushnumber(L, value);
	return 1;
}

int set_string_return(lua_State* L, std::string value) {
	lua_pushstring(L, value.c_str());
	return 1;
}
	
}
}
