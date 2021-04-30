#pragma once

#include "lua_fwd.hpp"
#include <string>
#include <tuple>
#include <type_traits>

namespace luabind {
namespace {

struct call_helper {
public:
	call_helper(lua_State* L)
		: L_(L)
	{}

	lua_State* L_;
};

template<typename T>
call_helper& operator&(call_helper& helper, T obj) {
	detail::push_value(helper.L_, obj);
	return helper;
}

}

template<typename Md, typename... Args>
typename Md::result_type call_typed_method(lua_State* L, Args... args) {
	detail::push_name(L, Md::name());
	call_helper h(L);
	auto _ = (h & ... & args);
	constexpr bool has_return = !std::is_void<typename Md::result_type>::value;
	detail::call_impl(L, std::tuple_size<std::tuple<Args...>>::value, has_return ? 1 : 0);
	if constexpr(has_return) {
		return get_parameter(L, 1, std::declval<typename Md::result_type>());
	}
}

template<typename Md, typename... Args>
typename Md::result_type call_named_method(lua_State* L, std::string fn, Args... args) {
	typename Md::result_type ret;
	detail::push_name(L, fn.c_str());
	call_helper h(L);
	(h & ... & args);
	constexpr bool has_return = !std::is_void<typename Md::result_type>::value;
	detail::call_impl(L, std::tuple_size<std::tuple<Args...>>::value, has_return ? 1 : 0);
	if constexpr(has_return) {
		ret = get_parameter(L, 1, std::declval<typename Md::result_type>());
	}
	return ret;
}

}