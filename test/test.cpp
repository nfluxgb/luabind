#include "luabind/luabind.hpp"
#include <iostream>

// Host -> LUA 
DEFINE_METHOD(INIT_PLUGIN, "init", void, int);
DEFINE_METHOD(SHUTDOWN_PLUGIN, "shutdown", void, void);
typedef std::tuple<INIT_PLUGIN, SHUTDOWN_PLUGIN> lua_methods;

// LUA -> HOST
DEFINE_METHOD(PRINT_MESSAGE, "print_message", void, std::string);
DEFINE_METHOD(ANNOUNCE, "announce", int, std::string);
DEFINE_METHOD(GET_OBJECT, "get_object", luabind::key_value_store, int);
DEFINE_METHOD(GET_ALL_OBJECTS, "get_all_objects", luabind::key_value_store);
DEFINE_METHOD(GET_ALL_NUMBERS, "get_all_numbers", luabind::key_value_store);
typedef std::tuple<PRINT_MESSAGE, ANNOUNCE, GET_OBJECT, GET_ALL_OBJECTS, GET_ALL_NUMBERS> host_methods;

void print(std::string const& msg) {
	std::cout << msg << std::endl;
}

struct myobject {
public:
	myobject() {}

	myobject(std::string name)
		: name(name)
	{}

	template<typename Ar>
	void to_archive(Ar& ar) {
		ar & std::make_pair("name", std::ref(name));
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
		objects_.push_back(std::make_shared<myobject>("test"));
	}

	myobject get_object(int id) {
		return *objects_.back();
	}

	std::vector<std::shared_ptr<myobject>> get_objects() {
		return objects_;
	}

	std::array<int, 32> get_numbers() {
		return std::array<int, 32>();
	}
private:
	std::vector<std::shared_ptr<myobject>> objects_;
};

int main(int argc, char* argv[]) {
	try {
		lua_State* L = luaL_newstate();
		luaL_openlibs(L);

		test_data td;

		luabind::callback_dispatcher dispatcher;
		dispatcher.bind_callback<PRINT_MESSAGE>(print);
		dispatcher.bind_callback<GET_OBJECT>([&](int id) {
			return luabind::map_object(td.get_object(id), {"name"});
		});
		dispatcher.bind_callback<GET_ALL_OBJECTS>([&]() {
			return luabind::map_container(td.get_objects(), {"name"});
		});
		dispatcher.bind_callback<GET_ALL_NUMBERS>([&]() {
			return luabind::map_array(td.get_numbers());
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
