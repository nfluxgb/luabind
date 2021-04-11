#include "luabind/method_call.hpp"
#include <lua.hpp>
#include <iostream>

namespace luabind {
namespace detail {


void push_number_parameter(lua_State* L, double value)
{
	lua_pushnumber(L, value);
}

void push_string_parameter(lua_State* L, std::string value)
{
	lua_pushstring(L, value.c_str());
}

double get_number_return(lua_State* L)
{
	return lua_tonumber(L, 1);
}

std::string get_string_return(lua_State* L)
{
	return std::string(lua_tostring(L, 1));
}

void push_name(lua_State* L, char const* fn)
{
	lua_getglobal(L, fn);
}

void call_impl(lua_State* L, std::size_t args, std::size_t returns)
{
	if(lua_pcall(L, args, returns, 0) != 0) {
		throw std::runtime_error(std::string(lua_tostring(L, -1)));
	}
}

}
}
