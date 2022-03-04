#pragma once

#include <iostream>

class Trackable
{
public:
	Trackable(const std::string& info){}
	Trackable() {}
	void* operator new(std::size_t size);
	void operator delete(void *ptr);
	void* operator new[](std::size_t size);
	void operator delete[](void *ptr);

	//for use with placement new
	void* operator new(std::size_t size, void* ptr){ return ptr; };
	void operator delete(void *ptr, void*ptr2){};
	void* operator new[](std::size_t size, void* ptr){ return ptr; };
	void operator delete[](void *ptr, void* ptr2){};

	const std::string& getDebugSource() { static std::string junk; return junk; }

private:
};
