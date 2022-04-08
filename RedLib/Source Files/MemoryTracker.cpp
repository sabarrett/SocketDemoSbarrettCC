#include "MemoryTracker.h"
#include <iostream>

using namespace std;

int MemoryTracker::msAllocationNum = 0;
MemoryTracker* MemoryTracker::mspInstance = nullptr;

MemoryTracker* MemoryTracker::getInstance()
{
	if (!mspInstance)
	{
		mspInstance = new MemoryTracker();
	}
	return mspInstance;
}

void MemoryTracker::cleanupInstance()
{
	if (mspInstance)
	{
		mspInstance->reportAllocations(cout);

		delete mspInstance;
		mspInstance = nullptr;
	}
}

void MemoryTracker::addAllocation(void* ptr, size_t size)
{
	AllocationRecord rec(msAllocationNum, size);
	pair<void*, AllocationRecord> pair(ptr, rec);
	mAllocaitons.insert(pair);
	msAllocationNum++;
}

void MemoryTracker::removeAllocation(void* ptr)
{
	unordered_map<void*, AllocationRecord>::iterator iter = mAllocaitons.find(ptr);
	if (iter != mAllocaitons.end())
		mAllocaitons.erase(iter);
}

void MemoryTracker::reportAllocations(ostream& stream)
{
	stream << endl << "Current Memory Allocations:" << endl;

	unordered_map<void*, AllocationRecord>::iterator iter;
	for (iter = mAllocaitons.begin(); iter != mAllocaitons.end(); ++iter)
	{
		stream << "Address:" << iter->first << " Size:" << iter->second.size << " Num:" << iter->second.num << endl;
	}
}