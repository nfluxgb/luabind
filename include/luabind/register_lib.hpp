#pragma once

#include <lua.hpp>
#include <vector>
#include <tuple>
#include <iostream>

namespace luabind {
namespace detail {

template<typename CallbackT, typename... Args>
void push_definition(std::vector<luaL_Reg>& defs, std::tuple<Args...> dummy) {
	defs.push_back(luaL_Reg{0, 0});
}

template<typename CallbackT, typename T, typename... Args>
void push_definition(std::vector<luaL_Reg>& defs, std::tuple<T, Args...> dummy) {
	defs.push_back(luaL_Reg{T::name(), &CallbackT::template handle<T>});
	push_definition<CallbackT, Args...>(defs, std::tuple<Args...>());
}

}

template<typename CallbackT, typename... Args>
void register_lib(lua_State* L, char const* name, std::tuple<Args...> dummy) {
	std::vector<luaL_Reg> definitions;
	detail::push_definition<CallbackT, Args...>(definitions, std::tuple<Args...>());
	luaL_register(L, name, definitions.data());
}

}
