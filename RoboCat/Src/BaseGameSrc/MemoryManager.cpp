#include "MemoryManager.h"


MemoryManager::MemoryManager()
{

}
MemoryManager::~MemoryManager()
{
	cleanup();
}

//Initializing the memory manager
void MemoryManager::init()
{
	mpMemoryPoolSmall = new MemoryPool(10000, 128);
	mpMemoryPoolMed = new MemoryPool(10000, 1024); //creating the medium memory pool
	mpMemoryPoolLarge = new MemoryPool(640, 1048576); //creating the large memory pool

	//Caluclates the total maximum capacity availible using the max object size and total number of free objects from each pool
	mTotalCapacity = (mpMemoryPoolSmall->getNumFreeObjects() * mpMemoryPoolSmall->getMaxObjectSize()) +
		(mpMemoryPoolMed->getNumFreeObjects() * mpMemoryPoolMed->getMaxObjectSize()) +
		(mpMemoryPoolLarge->getNumFreeObjects() * mpMemoryPoolLarge->getMaxObjectSize());

	mTotalAllocated = 0;
	mMaxCapacity = mTotalCapacity;
	mTotalWaste = 0;
}
//cleans up and resets the memory manager
void MemoryManager::cleanup()
{

	reset();

	//deletes all the memory pools
	delete(mpMemoryPoolSmall);
	delete(mpMemoryPoolMed);
	delete(mpMemoryPoolLarge);

	mpMemoryPoolSmall = nullptr;
	mpMemoryPoolMed = nullptr;
	mpMemoryPoolLarge = nullptr;

}

//resets the data and frees all the space in the memory pools
void MemoryManager::reset()
{
	//resets all the pools
	mpMemoryPoolSmall->reset();
	mpMemoryPoolMed->reset();
	mpMemoryPoolLarge->reset();

	//Calculates total free capacity when all pools are empty
	mTotalCapacity = (mpMemoryPoolSmall->getNumFreeObjects() * mpMemoryPoolSmall->getMaxObjectSize()) +
		(mpMemoryPoolMed->getNumFreeObjects() * mpMemoryPoolMed->getMaxObjectSize()) +
		(mpMemoryPoolLarge->getNumFreeObjects() * mpMemoryPoolLarge->getMaxObjectSize());
	mTotalAllocated = 0;
	mMaxCapacity = mTotalCapacity;
	mTotalWaste = 0;

}

//allocates an object for the passed in amount of memory
Byte* MemoryManager::allocate(unsigned int memory)
{
	Byte* ptr = nullptr;
	unsigned int size;

	//If-else statements to determine which pool to use
	if (memory <= mpMemoryPoolSmall->getMaxObjectSize())
	{
		size = mpMemoryPoolSmall->getMaxObjectSize(); //Sets size to the pool's max object size
		ptr = mpMemoryPoolSmall->allocateObject(); //allocates the object and sets the pointer
		mTotalAllocated += size; //updates the allocated data
		mTotalWaste += size - memory; //updates the waste data

	}
	else if (memory <= mpMemoryPoolMed->getMaxObjectSize())
	{
		size = mpMemoryPoolMed->getMaxObjectSize(); //Sets size to the pool's max object size
		ptr = mpMemoryPoolMed->allocateObject(); //allocates the object and sets the pointer
		mTotalAllocated += size; //updates the allocated data
		mTotalWaste += size - memory; //updates the waste data
	}
	else if (memory <= mpMemoryPoolLarge->getMaxObjectSize())
	{
		size = mpMemoryPoolLarge->getMaxObjectSize(); //Sets size to the pool's max object size
		ptr = mpMemoryPoolLarge->allocateObject(); //allocates the object and sets the pointer
		mTotalAllocated += size; //updates the allocated data
		mTotalWaste += size - memory; //updates the waste data
	}
	mTotalCapacity = mMaxCapacity - mTotalAllocated; //updates the total free capacity data

	return ptr;

}

//deallocates the passed in byte
void MemoryManager::deallocate(Byte* byte)
{
	//if else statements checking to see which pool the byte is contained in
	if (mpMemoryPoolSmall->contains(byte))
	{
		mpMemoryPoolSmall->freeObject(byte); //Frees the object at the passed in byte
		mTotalAllocated -= mpMemoryPoolSmall->getMaxObjectSize(); //updates the total allocated data
	}
	else if (mpMemoryPoolMed->contains(byte))
	{
		mpMemoryPoolMed->freeObject(byte); //Frees the object at the passed in byte
		mTotalAllocated -= mpMemoryPoolMed->getMaxObjectSize(); //updates the total allocated data
	}
	else if (mpMemoryPoolMed->contains(byte))
	{
		mpMemoryPoolLarge->freeObject(byte); //Frees the object at the passed in byte
		mTotalAllocated -= mpMemoryPoolLarge->getMaxObjectSize(); //updates the total allocated data
	}
	mTotalCapacity = mMaxCapacity - mTotalAllocated; //updates the total capacity data
}


