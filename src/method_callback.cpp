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

int push_return_value(lua_State* L, double value) {
	lua_pushnumber(L, value);
	return 1;
}

int push_return_value(lua_State* L, std::string value) {
	lua_pushstring(L, value.c_str());
	return 1;
}

int push_return_value(lua_State* L, container_mapper_ptr p) {
	(*p)(L);
	return 1;
}

int push_return_value(lua_State* L, object_mapper_ptr p) {
	(*p)(L);
	return 1;
}
	
}
}
