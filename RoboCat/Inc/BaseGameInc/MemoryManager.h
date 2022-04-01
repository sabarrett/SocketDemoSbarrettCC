#pragma once
#include <iostream>
#include <MemoryPool.h>
#include <MemoryTracker.h>

using namespace std;

class MemoryManager : public Trackable {
public:
	MemoryManager();
	~MemoryManager();

	//Initialization and cleanup functions
	void init();
	void cleanup();

	//Function to reset the memory allocations
	void reset();

	//Allocates memory to appropriate memoryPool
	Byte* allocate(unsigned int);

	//Deallocates memory from appropriate memoryPool
	void deallocate(Byte*);

	//returns total allocated memory
	unsigned int getTotalAllocated() { return mTotalAllocated; };

	//returns total capacity
	unsigned int getTotalCapacity() { return mTotalCapacity; };

	//returns total waste
	unsigned int getTotalWaste() { return mTotalWaste; };

private:
	MemoryPool* mpMemoryPoolSmall; //Pointer to small memory pool
	MemoryPool* mpMemoryPoolMed; //Pointer to medium memory pool
	MemoryPool* mpMemoryPoolLarge; //pointer to large memory pool

	unsigned int mTotalCapacity;
	unsigned int mTotalAllocated;

	unsigned int mTotalWaste;

	unsigned int mMaxCapacity;



};
