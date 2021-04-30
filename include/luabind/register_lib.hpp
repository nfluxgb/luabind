#pragma once

#include "method_definition.hpp"
#include <lua.hpp>
#include <vector>
#include <tuple>
#include <iostream>

namespace luabind {
namespace detail {

template<typename CallbackT>
struct definition_helper {
public:
	typedef void result_type;

	definition_helper(std::vector<luaL_Reg>& definitions)
		: definitions_(definitions)
	{}

	void operator()()
	{
		definitions_.push_back(luaL_Reg{0, 0});
	}

	template<typename Md, typename... Args>
	void operator()(Md, Args... defs) {
		definitions_.push_back(luaL_Reg{Md::name(), &CallbackT::template handle<Md>});
		(*this)(defs...);
	}

	std::vector<luaL_Reg>& definitions_;
};
}

template<typename CallbackT, typename TupleT>
void register_lib(lua_State* L, char const* name, TupleT dummy) {
	std::vector<luaL_Reg> definitions;
	detail::definition_helper<CallbackT> helper(definitions);
	std::apply(helper, dummy);
	luaL_register(L, name, definitions.data());
}

}
