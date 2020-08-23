#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <cassert>
#include <functional>

class Entity;
class Component {
public:
	virtual void Create() {

	}
	virtual ~Component() {

	}
	Entity* entity;
};

class ComponentA final : public Component {
public:
	void Create() {
		name = "create";
	}
	virtual ~ComponentA() {

	}
	std::string name = "windows";
};

class ComponentB final : public Component {
public:
	void Create() {
		fps = 100.f;
	}
	virtual ~ComponentB() {

	}
	float fps = 60.f;
};
//common object pool
template <typename T>
class ObjectPool {
public:
	ObjectPool(uint32_t maxSize) : kMaxSize(maxSize) {
		deleter = [](T* cs) {
			delete[] cs;
		};
	}
	~ObjectPool() {
		deleter(objects);
	}
	void allocate() {
		objects = new T[kMaxSize];
	}
	const T& at(uint32_t i) const {
		if (i < size) {
			return objects[i];
		}
		assert(i < size);
		return objects[0];
	}

	T& at(uint32_t i) {
		if (i < size) {
			return objects[i];
		}
		assert(i < size);
		return objects[0];
	}

	T* create() {
		if (kMaxSize > size) {
			return &objects[size++];
		} else {
			//reallocate action
			return nullptr;
		}
	}
	void remove(const T* t) {
		if (t == nullptr) {
			return;
		}
		uint32_t index = t - objects;
		if (size > 0) {
			objects[index] = objects[size - 1];
		}
		--size;
	}
	uint32_t getSize() const {
		return size;
	}
	uint32_t getMaxSize() const {
		return kMaxSize;
	}
private:
	T* objects = nullptr;
	void(*deleter)(T*);
	uint32_t size = 0;
	const uint32_t kMaxSize;
};



template<>
class ObjectPool<Component>{
public:
	//init but not allocate memory.
	ObjectPool(uint32_t maxSize) : kMaxSize(maxSize) {
		deleter = [](Component* cs) {
			delete[] cs;
		};
	}

	//default constructor, maxSize are set to 64.
	ObjectPool() : ObjectPool(64) {}
	
	//free memory with deleter.
	~ObjectPool() {
		deleter(objects);
	}

	//allocate memory
	template<typename Derived>
	void allocate() {
		objects = new Derived[kMaxSize];
		deleter = [](Component* cs) {
			delete[] static_cast<Derived*>(cs);
		};
	}

	//get constant value in [i].
	template<typename Derived>
	const Derived& at(uint32_t i) const {
		if (i < size) {
			return (static_cast<Derived*>(objects))[i];
		}
		assert(i < size);
		return (static_cast<Derived*>(objects))[0];
	}

	//get value in [i].
	template<typename Derived>
	Derived& at(uint32_t i) {
		if (i < size) {
			return (static_cast<Derived*>(objects))[i];
		}
		assert(i < size);
		return (static_cast<Derived*>(objects))[0];
	}
	//create available object.
	template<typename Derived>
	Derived* create() {
		if (kMaxSize > size) {
			return &((static_cast<Derived*>(objects))[size++]);
		} else {
			//reallocate action
			return nullptr;
		}
	}

	//remove a available object, then move last object into the removed one.
	template<typename Derived>
	void remove(Derived*& c) {
		if (c == nullptr) {
			return;
		}
		Derived* real = static_cast<Derived*>(objects);
		uint32_t index = c - real;
		if (size > 0 && index >= 0) {
			real[index] = real[size - 1];
		}
		--size;
		c = nullptr;
	}

	//get object size.
	uint32_t getSize() const {
		return size;
	}

	//get max size.
	uint32_t getMaxSize() const {
		return kMaxSize;
	}
private:
	Component* objects = nullptr;
	void(*deleter)(Component*);
	uint32_t size = 0;
	const uint32_t kMaxSize;
};









int main() {
	ObjectPool<int> ip(10);
	ip.allocate();
	for (size_t i = 0; i < 10; i++) {
		auto x = ip.create();
		*x = i;
	}
	ip.remove(&ip.at(2));
	for (uint32_t i = 0; i < ip.getSize(); i++) {
		std::cout << ip.at(i) << std::endl;
	}

	ObjectPool<Component>* cp = new ObjectPool<Component>[2];
	cp[0].allocate<ComponentA>();
	cp[1].allocate<ComponentB>();
	auto cp1 = cp[0].create<ComponentA>();
	cp1->name = "cp1";
	auto cp2 = cp[0].create<ComponentA>();
	cp2->name = "cp2";
	auto cp3 = cp[0].create<ComponentA>();
	cp3->name = "cp3";
	auto cp4 = cp[0].create<ComponentA>();
	cp4->name = "cp4";
	auto cp5 = cp[0].create<ComponentA>();
	cp5->name = "cp5";
	cp[0].remove(cp2);
	std::cout << cp[0].getSize() << std::endl;
	cp[0].remove(cp2);
	std::cout << cp[0].getSize() << std::endl;
	cp[0].remove(cp3);
	std::cout << cp[0].getSize() << std::endl;
	cp[0].remove(cp1);
	std::cout << cp[0].getSize() << std::endl;
}