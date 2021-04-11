#pragma once

#include "method_callback.hpp"
#include "lua_fwd.hpp"
#include <memory>

namespace luabind {

class method_binding_base {
public:
	virtual ~method_binding_base() {}
	virtual int call(lua_State* L) = 0;
};
typedef std::shared_ptr<method_binding_base> method_binding_ptr;

template<typename Md, typename Fn>
class method_binding : public method_binding_base {
public:
	method_binding(Fn f)
		: f_(f)
	{}

	int call(lua_State* L) override {
		return handle_callback<Md, Fn>(L, f_);
	}

private:
	Fn f_;
};

template<typename MD, typename T, typename Fn>
class method_construct_binding : public method_binding_base {
public:
	method_construct_binding(Fn f)
		: f_(f)
	{}

	int call(lua_State* L) override {
		return handle_construct_callback<MD, T, Fn>(L, f_);
	}

private:
	Fn f_;
};

}
