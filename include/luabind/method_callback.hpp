#pragma once

#include "lua_fwd.hpp"
#include <string>
#include <tuple>

namespace luabind {
namespace detail {

double get_number_parameter(lua_State* L, std::size_t n);
std::string get_string_parameter(lua_State* L, std::size_t n);

template<typename T, typename U = std::is_arithmetic<T>>
T get_parameter(lua_State* L, std::size_t n, typename std::enable_if<U::value>::type* = 0) {
	return (T) get_number_parameter(L, n);
}

template<typename T, typename U = std::is_arithmetic<T>>
T get_parameter(lua_State* L, std::size_t n, typename std::enable_if<!U::value>::type* = 0) {
	return (T) get_string_parameter(L, n);
}

int set_number_return(lua_State* L, double value);
int set_string_return(lua_State* L, std::string value);

template<typename T, typename U = std::is_arithmetic<T>>
int push_return_value(lua_State* L, T t, typename std::enable_if<U::value>::type* = 0) {
	return set_number_return(L, (double) t);
}

template<typename T, typename U = std::is_arithmetic<T>>
int push_return_value(lua_State* L, T t, typename std::enable_if<!U::value>::type* = 0, ...) {
	return set_string_return(L, (std::string) t);
}

template<typename Md, typename Tuple>
typename Md::tuple_type handle_callback_impl(lua_State* L, Tuple in, std::tuple<> out)
{
	return in;
}

template<typename Md, typename Tuple, typename T, typename... Args>
typename Md::tuple_type handle_callback_impl(lua_State* L, Tuple in, std::tuple<T, Args...> out) {
	T val = get_parameter<T>(L, 1 + std::tuple_size<Tuple>::value);
	return handle_callback_impl<Md>(L, std::tuple_cat(in, std::tuple<T>(val)), std::tuple<Args...>());
}

template<typename Fn, typename Tup, typename U = decltype(std::apply(std::declval<Fn>(), std::declval<Tup>()))>
int handle_callback_return(lua_State* L, Fn f, Tup tup, typename std::enable_if<!std::is_void<U>::value>::type* = 0) {
	return detail::push_return_value(L, std::apply(f, tup));
}

template<typename Fn, typename Tup, typename U = decltype(std::apply(std::declval<Fn>(), std::declval<Tup>()))>
int handle_callback_return(lua_State* L, Fn f, Tup tup, typename std::enable_if<std::is_void<U>::value>::type* = 0) {
	std::apply(f, tup);
	return 0;
}

template<typename T, typename Fn, typename Tup, typename U = decltype(std::declval<Fn>()(std::make_from_tuple<T>(std::declval<Tup>())))>
int handle_construct_callback_return(lua_State* L, Fn f, Tup tup, typename std::enable_if<!std::is_void<U>::value>::type* = 0) {
	return detail::push_return_value(L, f(std::make_from_tuple<T>(tup)));
}

template<typename T, typename Fn, typename Tup, typename U = decltype(std::declval<Fn>()(std::make_from_tuple<T>(std::declval<Tup>())))>
int handle_construct_callback_return(lua_State* L, Fn f, Tup tup, typename std::enable_if<std::is_void<U>::value>::type* = 0) {
	f(std::make_from_tuple<T>(tup));
	return 0;
}

}

template<typename Md, typename Fn>
int handle_callback(lua_State* L, Fn func) {
	int ret = 0;
	typename Md::tuple_type tup = 
		detail::handle_callback_impl<Md>(L, std::tuple<>(), typename Md::tuple_type());
	ret = detail::handle_callback_return(L, func, tup);
	return ret;
}

template<typename Md, typename T, typename Fn>
int handle_construct_callback(lua_State* L, Fn func) {
	int ret = 0;
	typename Md::tuple_type tup = 
		detail::handle_callback_impl<Md>(L, std::tuple<>(), typename Md::tuple_type());
	ret = detail::handle_construct_callback_return<T>(L, func, tup);
	return ret;
}

}
