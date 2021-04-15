#pragma once

#include "method_call.hpp"
#include <lua.hpp>
#include <algorithm>
#include <memory>

namespace luabind {
namespace detail {

class object_mapper_base {
public:
	virtual ~object_mapper_base() {}
	virtual void operator()(lua_State* L) = 0;
};

template<typename TupT>
class object_mapper : public object_mapper_base {
public:
	typedef std::array<std::string, std::tuple_size<TupT>::value> names_type;
	typedef TupT tuple_type;
public:
	explicit object_mapper(names_type names, TupT tuple = TupT())
		: names_(names)
		, tuple_(tuple)
	{}

	void operator()() {}

	template<typename T, typename... Args>
	void operator()(T head, Args... tail) {
		if(std::find(std::begin(names_), std::end(names_), head.first) != std::end(names_)) {
			push_argument(L_, head.second);
			lua_setfield(L_, -2, head.first);
		}
		(this->operator())(tail...);
	}

	void operator()(lua_State* L) override {
		L_ = L;
		lua_newtable(L_);
		std::apply(*this, tuple_);
	}

	names_type names_;
	tuple_type tuple_;
	lua_State* L_;
};

class container_mapper_base {
public:
	virtual ~container_mapper_base() {}
	virtual void operator()(lua_State* L) = 0;
};

template<typename T, typename S = std::is_convertible<T, nullptr_t>>
auto to_tuple(T obj, typename std::enable_if<S::value>::type* = 0) {
	return obj->to_tuple();
}

template<typename T, typename S = std::is_convertible<T, nullptr_t>>
auto to_tuple(T obj, typename std::enable_if<!S::value>::type* = 0) {
	return obj.to_tuple();
}


template<typename ContT, typename NamesT>
class container_mapper : public container_mapper_base {
public:
	container_mapper(ContT container, NamesT names)
			: container_(container)
			, names_(names)
	{}

	void operator()(lua_State* L) override {
		lua_createtable(L, 0, container_.size());
		for(auto it = std::begin(container_); it != std::end(container_); ++it) {
			push_argument(L, std::distance(std::begin(container_), it));
			auto tup = to_tuple(*it);
			object_mapper<decltype(tup)> mapper(names_, tup);
			mapper(L);
			lua_settable(L, -3);
		}
	}

	ContT container_;
	NamesT names_;
};

class array_mapper_base {
public:
	virtual ~array_mapper_base() {}
	virtual void operator()(lua_State* L) = 0;
};

template<typename TupT>
class array_mapper : public array_mapper_base {
public:
	typedef TupT tuple_type;
public:
	explicit array_mapper(TupT tuple = TupT())
		: tuple_(tuple)
	{}

	void operator()() {}

	template<typename T, typename... Args>
	void operator()(T head, Args... tail) {
		push_argument(L_, head.first);
		push_argument(L_, head.second);
		lua_settable(L_, -3);
		(this->operator())(tail...);
	}

	void operator()(lua_State* L) override {
		L_ = L;
		lua_newtable(L_);
		std::apply(*this, tuple_);
	}

	tuple_type tuple_;
	lua_State* L_;
};

template<int N, typename ArrT, typename TupT, typename S = std::tuple_size<ArrT>>
auto to_tuple(ArrT arr, TupT tuple, typename std::enable_if<S::value == N>::type* = 0) {
	return tuple;
}

template<int N, typename ArrT, typename TupT, typename S = std::tuple_size<ArrT>>
auto to_tuple(ArrT arr, TupT tuple, typename std::enable_if<S::value != N>::type* = 0) {
	return to_tuple<N+1>(arr, std::tuple_cat(tuple, std::make_tuple(std::make_pair(N, std::get<N>(arr)))));
}

}

typedef std::shared_ptr<detail::object_mapper_base> object_mapper_ptr;

template<int N, typename ObjT>
object_mapper_ptr map_object(ObjT obj, std::array<std::string, N> field_names) {
	return std::make_shared<detail::object_mapper<decltype(obj.to_tuple())>>(field_names, obj.to_tuple());
}

typedef std::shared_ptr<detail::container_mapper_base> container_mapper_ptr;

template<int N, typename ContT>
container_mapper_ptr map_container(ContT cont, std::array<std::string, N> names) {
	return std::make_shared<detail::container_mapper<ContT, decltype(names)>>(cont, names);
}

typedef std::shared_ptr<detail::array_mapper_base> array_mapper_ptr;

template<int N, typename T>
array_mapper_ptr map_array(std::array<T, N> obj) {
	auto tup = detail::to_tuple<0>(obj, std::tuple<>());
	return std::make_shared<detail::array_mapper<decltype(tup)>>(tup);
}

}
