#pragma once

#include "method_call.hpp"
#include <lua.hpp>
#include <algorithm>
#include <memory>
#include <tuple>

namespace luabind {
namespace {

struct object_mapper {
public:
	object_mapper(key_value_store& kvs, std::vector<std::string> names)
		: kvs_(kvs)
	{}

	key_value_store& kvs_;
	std::vector<std::string> names_;
};

template<typename T>
object_mapper& operator&(object_mapper& archive, T&& obj) {
	char const* field_name = obj.first;
	auto it = archive.names_.begin();
	for(; it != archive.names_.end() && *it != field_name; ++it);
	if(archive.names_.empty() || it != archive.names_.end()) {
		if constexpr(std::is_arithmetic<decltype(obj.second)>::value) {
			archive.kvs_.push_back(std::make_pair(obj.first, (double) obj.second));
		}
		else if constexpr(std::is_convertible<decltype(obj.second), std::string>::value) {
			archive.kvs_.push_back(std::make_pair(obj.first, (std::string) obj.second));
		}
		else {
			throw std::runtime_error("unsupported type");
		}
	}
	return archive;
}

template<typename T>
struct is_shared_ptr : std::false_type {};

template<typename T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

}

template<typename ObjT>
key_value_store map_object(ObjT obj, std::vector<std::string> field_names) {
	key_value_store ret;
	object_mapper mapper(ret, field_names);
	obj.to_archive(mapper);
	return ret;
}

template<typename ContT>
std::vector<key_value_store> map_container(ContT cont, std::vector<std::string> field_names) {
	std::vector<key_value_store> ret;
	for(auto&& obj : cont) {
		key_value_store kvs;
		object_mapper mapper(kvs, field_names);
		if constexpr(is_shared_ptr<typename ContT::value_type>::value) {
			obj->to_archive(mapper);
		}
		else {
			obj.to_archive(mapper);	
		}
		ret.push_back(kvs);
	}
	return ret;
}

}
