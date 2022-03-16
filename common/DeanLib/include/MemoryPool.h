#pragma once

#include "DeanLibDefines.h"
#include "CircularQueue.h"
#include "Trackable.h"


class MemoryPool: public Trackable
{
public:
	MemoryPool(unsigned int maxNumObjects, unsigned int objectSize);
	~MemoryPool() { free(mMemory); delete mpFreeList; };

	void reset();//doesn't reallocate memory but does reset free list and num allocated objects

	Byte* allocateObject();
	void freeObject(Byte* ptr);

	inline Uint32 getMaxObjectSize(){ return mObjectSize; };
	inline Uint32 getNumFreeObjects(){ return mMaxNumObjects - mNumAllocatedObjects; };

private:
	Byte* mMemory;
	Byte* mHighestValidAddress;
	Uint32 mMaxNumObjects;
	Uint32 mNumAllocatedObjects;
	Uint32 mObjectSize;
	CircularQueue<Byte*>* mpFreeList;

	void createFreeList();
};