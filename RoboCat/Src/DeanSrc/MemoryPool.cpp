#include "MemoryPool.h"
#include <cstdlib>
#include <assert.h>
#include <iostream>

using namespace std;

//got this algorithm from: http://www.exploringbinary.com/ten-ways-to-check-if-an-integer-is-a-power-of-two-in-c/
int isPowerOfTwo(unsigned int x)
{
	return ((x != 0) && !(x & (x - 1)));
}

unsigned int getClosestPowerOf2LargerThan(unsigned int num)
{
	static Uint32 powersOf2[32];
	static bool arrayInitted = false;

	//init an array containing all the powers of 2 
	//(as it is static this should only run the first time this function is called)
	if (!arrayInitted)
	{
		for (Uint32 i = 0; i < 32; i++)
		{
			powersOf2[i] = 1 << i;
		}
	}

	//find the 1st power of 2 which is bigger than or equal to num
	for (Uint32 i = 0; i < 32; i++)
	{
		if ( powersOf2[i] >= num )
			return powersOf2[i];
	}

	//failsafe
	return 0;
	
}

MemoryPool::MemoryPool(Uint32 maxNumObjects, Uint32 objectSize)
{
	//make objectSize a power of 2 - used for padding
	objectSize = getClosestPowerOf2LargerThan(objectSize);
	if (objectSize < 4)
	{
		objectSize = 4;
	}

	//allocate the memory
	mMemory = (Byte*)malloc(objectSize * maxNumObjects);

	//set member variables
	mMaxNumObjects = maxNumObjects;
	mNumAllocatedObjects = 0;
	mObjectSize = objectSize;
	mHighestValidAddress = mMemory + ((maxNumObjects - 1) * objectSize);

	//allocate the free list
	mpFreeList = new CircularQueue<Byte*>(mMaxNumObjects);

	//create the free list
	createFreeList();
}

void MemoryPool::reset()
{
	//clear the free list
	mpFreeList->reset();
	//create the free list again
	createFreeList();
	//reset count of allocated objects
	mNumAllocatedObjects = 0;
}

Byte* MemoryPool::allocateObject()
{
	if (mNumAllocatedObjects >= mMaxNumObjects)
	{
		return NULL;
	}

	mNumAllocatedObjects++;
	Byte* ptr;
	bool success = mpFreeList->popFront(ptr);
	if (success)
	{
		return ptr;
	}
	else
	{
		assert(false);
		return NULL;
	}
}

void MemoryPool::freeObject(Byte* ptr)
{
	//make sure that the address passed in is actually one managed by this pool
	if (contains(ptr))
	{
		//add address back to free list
		mpFreeList->pushBack(ptr);

		mNumAllocatedObjects--;
	}
	else
	{
		cout << "ERROR:  object freed from a pool that doesn't manage it\n";
		assert(ptr >= mMemory && ptr <= mHighestValidAddress);
	}
}

bool MemoryPool::contains(Byte* ptr) const
{
	return (ptr >= mMemory && ptr <= mHighestValidAddress);
}

void MemoryPool::createFreeList()
{
	for (Uint32 i = 0; i<mMaxNumObjects; i++)
	{
		mpFreeList->pushBack(mMemory + (i * mObjectSize));
	}

}
