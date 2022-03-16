#include "Trackable.h"
#include "MemoryTracker.h"

void* Trackable::operator new( std::size_t size )
{
	void* ptr = malloc(size);
	MemoryTracker::getInstance()->addAllocation( ptr, size );
	return ptr;
}

void Trackable::operator delete( void *ptr )
{
	MemoryTracker::getInstance()->removeAllocation(ptr);
	free(ptr);
}

void* Trackable::operator new[]( std::size_t size )
{
	void* ptr = malloc(size);
	MemoryTracker::getInstance()->addAllocation( ptr, size );
	return ptr;
}

void Trackable::operator delete[]( void *ptr )
{
	MemoryTracker::getInstance()->removeAllocation(ptr);
	free(ptr);
}