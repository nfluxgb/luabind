#pragma once

#include <any>
#include <list>
#include <string>

namespace luabind {

typedef std::list<std::pair<std::string const, std::any>> key_value_store;
typedef std::list<std::pair<int, std::any>> array_store;

}
