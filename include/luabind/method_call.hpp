#pragma once

#include "lua_fwd.hpp"
#include <string>
#include <tuple>
#include <type_traits>

namespace luabind {
namespace detail {

void push_number_parameter(lua_State* L, double value);
void push_string_parameter(lua_State* L, std::string value);

template<typename T, typename U = std::is_arithmetic<T>>
void push_argument(lua_State* L, T t, typename std::enable_if<U::value>::type* = 0) {
	push_number_parameter(L, (double) t);
}

template<typename T, typename U = std::is_arithmetic<T>>
void push_argument(lua_State* L, T t, typename std::enable_if<!U::value>::type* = 0) {
	push_string_parameter(L, (std::string) t);
}

inline void push_arguments(lua_State* L) {}

template<typename T, typename... Args>
void push_arguments(lua_State* L, T arg, Args... args) {
	push_argument(L, arg);
	push_arguments(L, args...);
}

double get_number_return(lua_State* L);
std::string get_string_return(lua_State* L);

template<typename T, typename U = std::is_arithmetic<T>>
T get_return_impl(lua_State* L, typename std::enable_if<U::value>::type* = 0) {
	return (T) get_number_return(L);
}

template<typename T, typename U = std::is_arithmetic<T>>
T get_return_impl(lua_State* L, typename std::enable_if<!U::value>::type* = 0) {
	return (T) get_string_return(L);
}

void push_name(lua_State* L, char const* fn);
void call_impl(lua_State* L, std::size_t args, std::size_t returns);

template<typename Md, typename U = std::is_void<typename Md::result_type>>
typename Md::result_type get_return(lua_State* L, typename std::enable_if<!U::value>::type* = 0) {
	return detail::get_return_impl<typename Md::result_type>(L);
}

template<typename Md, typename U = std::is_void<typename Md::result_type>>
typename Md::result_type get_return(lua_State* L, typename std::enable_if<U::value>::type* = 0) {
	return;
}

}

template<typename Md, typename... Args>
typename Md::result_type call_typed_method(lua_State* L, Args... args) {
	detail::push_name(L, Md::name());
	detail::push_arguments(L, args...);
	detail::call_impl(L, std::tuple_size<std::tuple<Args...>>::value,
		std::is_same<typename Md::result_type, void>::value ? 0 : 1);
	return detail::get_return<Md>(L);
}

template<typename Md, typename... Args>
typename Md::result_type call_named_method(lua_State* L, std::string fn, Args... args) {
	detail::push_name(L, fn.c_str());
	detail::push_arguments(L, args...);
	detail::call_impl(L, std::tuple_size<std::tuple<Args...>>::value, std::is_same<typename Md::result_type, void>::value ? 0 : 1);
	return detail::get_return<Md>(L);
}

}