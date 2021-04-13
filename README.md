# luabind
C++ library that provides support for type-safe bindings to lua

Example LUA:
```
function init(id)
	mylib.print_message("in init. got: " .. id)
	local announce_id = mylib.announce("henry")
	mylib.print_message("announce got " .. announce_id)
	print(mylib.get_object())
	print(mylib.get_all_objects())
end

function shutdown()
	mylib.print_message ("in shutdown")
end

```

Example Source:
```
#include "luabind/luabind.hpp"
#include <iostream>

// Host -> LUA 
DEFINE_METHOD(INIT_PLUGIN, "init", void, int);
DEFINE_METHOD(SHUTDOWN_PLUGIN, "shutdown", void, void);
typedef std::tuple<INIT_PLUGIN, SHUTDOWN_PLUGIN> lua_methods;

// LUA -> HOST
DEFINE_METHOD(PRINT_MESSAGE, "print_message", void, std::string);
DEFINE_METHOD(ANNOUNCE, "announce", int, std::string);
DEFINE_METHOD(GET_OBJECT, "get_object", luabind::object_mapper_ptr, int);
DEFINE_METHOD(GET_ALL_OBJECTS, "get_all_objects", luabind::container_mapper_ptr);
typedef std::tuple<PRINT_MESSAGE, ANNOUNCE, GET_OBJECT, GET_ALL_OBJECTS> host_methods;

void print(std::string const& msg) {
	std::cout << msg << std::endl;
}

struct myobject {
public:
	myobject(std::string name)
		: name(name)
	{}

	auto to_tuple() {
		return std::make_tuple(
			std::make_pair("name", name));
	}

	std::string name;
};

int announce(myobject const& obj) {
	std::cout << obj.name << " has arrived" << std::endl; 
	return 1;
}

class test_data {
public:
	test_data() {
		objects_.push_back(myobject("test"));
	}

	myobject get_object(int id) {
		return objects_.back();
	}

	std::vector<myobject> get_objects() {
		return objects_;
	}
private:
	std::vector<myobject> objects_;
};

int main(int argc, char* argv[]) {
	try {
		lua_State* L = luaL_newstate();
		luaL_openlibs(L);

		test_data td;

		luabind::callback_dispatcher dispatcher;
		dispatcher.bind_callback<PRINT_MESSAGE>(print);
		dispatcher.bind_callback<GET_OBJECT>([&](int id) {
			return luabind::map_object<1>(td.get_object(id), {"name"});
		});
		dispatcher.bind_callback<GET_ALL_OBJECTS>([&]() {
			return luabind::map_container<1>(td.get_objects(), {"name"});
		});
		dispatcher.bind_construct_callback<ANNOUNCE, myobject>(announce);

		luabind::register_lib<luabind::callback_dispatcher>(L, "mylib", host_methods());

		luaL_dofile(L, "test.lua");
		luabind::call_typed_method<INIT_PLUGIN>(L, 1);
		luabind::call_typed_method<SHUTDOWN_PLUGIN>(L);
	}
	catch(std::exception const& ex) {
		std::cerr << ex.what() << "\n";
	}
	return 0;
}
```

Example output:
```
in init. got: 1
henry has arrived
announce got 1
table: 0x55555558e0f0
table: 0x55555558d060
in shutdown
```
