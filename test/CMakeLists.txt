find_package(Lua 5.1 REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_executable(luabind_example test.cpp)

configure_file(test.lua ${CMAKE_CURRENT_SOURCE_DIR}/../bin/ COPYONLY)
include_directories(${LUA_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../include/)
target_link_libraries(luabind_example ${LUA_LIBRARIES} ${CMAKE_CURRENT_SOURCE_DIR}/../lib/libluabind.a)