#pragma once

#include <tuple>

namespace luabind {
namespace detail {

template<typename R, typename... Args>
struct method_target {
public:
	typedef R result_type;
	typedef std::tuple<Args...> tuple_type;
};

template<typename T, char const* Fn>
struct method_definition {
public:
	typedef typename T::result_type result_type;
	typedef typename T::tuple_type tuple_type;
	static char const* name() { 
		return Fn;
	}
};

}
}

#define DEFINE_METHOD(def_name, fn, ...) \
	inline extern char const def_name##_text[] = fn; \
	typedef luabind::detail::method_definition<luabind::detail::method_target<__VA_ARGS__>, def_name##_text> def_name
