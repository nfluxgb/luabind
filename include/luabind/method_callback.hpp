#pragma once

#include "lua_fwd.hpp"
#include <string>
#include <tuple>

namespace luabind {
namespace {

struct construct_object {
public:
	construct_object(lua_State* L)
		: L_(L)
		, offset_(1)
	{}

	lua_State* L_;
	int offset_;
};

template<typename T>
construct_object& operator&(construct_object& archive, T obj) {
	if constexpr(std::is_arithmetic<decltype(obj.second)>::value) {
		obj.second = detail::get_value(archive.L_, archive.offset_, double());
	}
	else if constexpr(std::is_convertible<decltype(obj.second), std::string>::value) {
		obj.second = detail::get_value(archive.L_, archive.offset_, std::string());
	}
	++archive.offset_;
	return archive;
}

struct callback_helper {
public:
	callback_helper(lua_State* L)
		: L_(L)
		, offset_(1)

	{}

	void operator()() {}

	template<typename T, typename... Args>
	void operator()(T&& obj, Args... args) {
		if constexpr(std::is_arithmetic<T>::value) {
			obj = detail::get_value(L_, offset_, double());
		}
		else if constexpr(std::is_convertible<T, std::string>::value) {
			obj = detail::get_value(L_, offset_, std::string());
		}
		++offset_;
		(*this)(args...);
	}

	lua_State* L_;
	int offset_;
};

}

template<typename Md, typename Fn>
int handle_callback(lua_State* L, Fn func) {
	int ret = 0;
	typename Md::tuple_type tup;
	callback_helper helper(L);
	if constexpr(!std::is_same<typename Md::tuple_type, std::tuple<>>::value) {
		std::apply(helper, tup);
	}
	if constexpr(std::is_void<decltype(std::apply(func,tup))>::value) {
		std::apply(func, tup);
	}
	else {
		detail::push_value(L, std::apply(func, tup));
		ret = 1;
	}
	return ret;
}

template<typename Md, typename T, typename Fn>
int handle_construct_callback(lua_State* L, Fn func) {
	int ret = 0;
	construct_object archive(L);
	T obj;
	obj.to_archive(archive);
	if constexpr(std::is_void<decltype(func(obj))>::value) {
		func(obj);
	}
	else {
		detail::push_value(L, func(obj));
		ret = 1;
	}
	return ret;
}

}
