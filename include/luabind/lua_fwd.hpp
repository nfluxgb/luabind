#pragma once

#include "key_value_store.hpp"
#include <type_traits>
#include <string>
#include <stdexcept>
#include <vector>

struct lua_State;

namespace luabind {
namespace detail {

double get_value(lua_State* L, int num, double default_value);
std::string get_value(lua_State* L, int num, std::string default_value);
void push_name(lua_State* L, std::string name);
void push_value(lua_State* L, double value);
void push_value(lua_State* L, std::string value);
void push_value(lua_State* L, key_value_store kvs);
void push_value(lua_State* L, array_store as);
void push_value(lua_State* L, std::vector<key_value_store> kvs);
void call_impl(lua_State* L, std::size_t args, std::size_t returns);

template<typename T>
void push_value(lua_State* L, T val) {
	if constexpr(std::is_arithmetic<T>::value) {
		push_value(L, (double) val);
	}
	else if constexpr(std::is_convertible<T, std::string>::value) {
		push_value(L, (std::string) val);
	}
	else if constexpr(std::is_convertible<T, key_value_store>::value) {
		push_value(L, (key_value_store) val);
	}
	else if constexpr(std::is_convertible<T, array_store>::value) {
		push_value(L, (array_store) val);
	}
	else {
		throw std::runtime_error("cannot convert type" + std::string(typeid(T).name()));
	}
}

}
}
