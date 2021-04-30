#pragma once

#include "method_binding.hpp"
#include <map>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <iostream>

namespace luabind {

struct callback_dispatcher {
public:
	callback_dispatcher() {
		instance() = this;
	}

	static callback_dispatcher*& instance() {
		static callback_dispatcher* ret = nullptr;
		return ret;
	}

	template<typename Md, typename Fn>
	void bind_callback(Fn f) {
		bound_methods_[typeid(Md)] = std::make_shared<method_binding<Md, Fn>>(f);
	}

	template<typename Md, typename T, typename Fn>
	void bind_construct_callback(Fn f) {
		bound_methods_[typeid(Md)] = std::make_shared<method_construct_binding<Md, T, Fn>>(f);
	}

	template<typename T>
	static int handle(lua_State* L) {
		int ret = 0;
		auto it = instance()->bound_methods_.find(typeid(T));
		if(it != instance()->bound_methods_.end()) {
			ret = it->second->call(L);
		}
		else {
			throw std::runtime_error("unknown function");
		}
		return ret;
	}
private:
	std::map<std::type_index, method_binding_ptr> bound_methods_;
};

}
