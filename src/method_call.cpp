#include "luabind/method_call.hpp"
#include <lua.hpp>
#include <iostream>

namespace luabind {
namespace detail {

double get_value(lua_State* L, int num, double default_value)
{
	default_value = lua_tonumber(L, num);
	return default_value;
}

std::string get_value(lua_State* L, int num, std::string default_value)
{
	default_value = std::string(lua_tostring(L, num));
	return default_value;
}

void push_name(lua_State* L, std::string fn)
{
	lua_getglobal(L, fn.c_str());
}

void push_value(lua_State* L, double value)
{
	lua_pushnumber(L, value);
}

void push_value(lua_State* L, std::string value)
{
	lua_pushstring(L, value.c_str());
}

void push_value(lua_State* L, key_value_store p) {
	lua_newtable(L);
	for(auto elem : p) {
		if(elem.second.type() == typeid(double)) {
			push_value(L, std::any_cast<double>(elem.second));
		}
		else if(elem.second.type() == typeid(std::string)) {
			push_value(L, std::any_cast<std::string>(elem.second));
		}
		lua_setfield(L, -2, elem.first.c_str());
	}
}

void push_value(lua_State* L, array_store as)
{
	lua_newtable(L);
	for(auto it = std::begin(as); it != std::end(as); ++it) {
		push_value(L, it->first);
		if(it->second.type() == typeid(double)) {
			push_value(L, std::any_cast<double>(it->second));
		}
		else if(it->second.type() == typeid(std::string)) {
			push_value(L, std::any_cast<std::string>(it->second));
		}
		lua_settable(L, -3);
	}
}

void push_value(lua_State* L, std::vector<key_value_store> p) {
	lua_createtable(L, 0, p.size());
	for(auto it = std::begin(p); it != std::end(p); ++it) {
		push_value(L, std::distance(std::begin(p), it));
		push_value(L, *it);
		lua_settable(L, -3);
	}
}

void call_impl(lua_State* L, std::size_t args, std::size_t returns)
{
	if(lua_pcall(L, args, returns, 0) != 0) {
		throw std::runtime_error(std::string(lua_tostring(L, -1)));
	}
}

}
}
