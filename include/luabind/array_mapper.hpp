#pragma once

#include "key_value_store.hpp"
#include <string>

namespace luabind {

template<typename ContT>
array_store map_array(ContT obj) {
	array_store ret;
	for(auto it = std::begin(obj); it != std::end(obj); ++it) {
		ret.push_back(std::make_pair(std::distance(std::begin(obj), it), std::any((double)*it)));
	}
	return ret;
}

}
