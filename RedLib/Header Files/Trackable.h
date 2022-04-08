//Inspired by Dean Lawson, Champlain College

#pragma once
#include <cstdlib>

class Trackable
{
public:
	Trackable() {}

	void* operator new(std::size_t size);
	void operator delete(void* ptr);
	void* operator new[](std::size_t size);
	void operator delete[](void* ptr);
};
