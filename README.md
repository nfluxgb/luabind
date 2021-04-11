# luabind
C++ library that provides support for type-safe bindings to lua

Example LUA:
```
function init(id)
	mylib.print_message("in init. got: " .. id)
	local announce_id = mylib.announce("henry")
	mylib.print_message("announce got " .. announce_id)
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
DEFINE_METHOD(SHUTDOWN_PLUGIN, "shutdown", void);
typedef std::tuple<INIT_PLUGIN, SHUTDOWN_PLUGIN> lua_methods;

// LUA -> HOST
DEFINE_METHOD(PRINT_MESSAGE, "print_message", void, std::string);
DEFINE_METHOD(ANNOUNCE, "announce", int, std::string);
typedef std::tuple<PRINT_MESSAGE, ANNOUNCE> host_methods;

struct myobject {
public:
	myobject(std::string name)
		: name(name)
	{}

	std::string name;
};

void print(std::string const& msg) {
	std::cout << msg << std::endl;
}

int announce(myobject const& obj) {
	std::cout << obj.name << " has arrived" << std::endl; 
	return 1;
}

int main(int argc, char* argv[]) {
	try {

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	luabind::callback_dispatcher dispatcher;
	dispatcher.bind_callback<PRINT_MESSAGE>(print);
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
in shutdown
```
