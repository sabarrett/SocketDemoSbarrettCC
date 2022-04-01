#pragma once

#include <iostream>
#include "RoboCatPCH.h"
class Trackable
{
public:
	Trackable() {}
	void* operator new(std::size_t size);
	void operator delete(void *ptr);
	void* operator new[](std::size_t size);
	void operator delete[](void *ptr);

	//for use with placement new
	void* operator new(std::size_t size, void* ptr) { UNREFERENCED_PARAMETER(size); return ptr; };
	void operator delete(void* ptr, void* ptr2) { UNREFERENCED_PARAMETER(ptr); UNREFERENCED_PARAMETER(ptr2); };
	void* operator new[](std::size_t size, void* ptr){ UNREFERENCED_PARAMETER(size); return ptr; };
	void operator delete[](void *ptr, void* ptr2){UNREFERENCED_PARAMETER(ptr); UNREFERENCED_PARAMETER(ptr2); };

	const std::string& getDebugSource() { static std::string junk; return junk; }

private:
};
