#pragma once

#include <unordered_map>
#include <iostream>


class MemoryTracker
{
public:
	static MemoryTracker* getInstance();

	void addAllocation( void* ptr, size_t size );
	void removeAllocation( void* ptr );
	
	void reportAllocations( std::ostream& stream );

private:
	struct AllocationRecord
	{
		AllocationRecord(int theNum, size_t theSize) : num(theNum), size(theSize) {};
		int num;
		size_t size;
	};


	MemoryTracker();
	~MemoryTracker();

	//copying not allowed
	MemoryTracker( const MemoryTracker& );
	MemoryTracker& operator=( const MemoryTracker& );

	std::unordered_map<void*,AllocationRecord> mAllocations;

	static int msAllocationNum;
	static MemoryTracker* mspInstance;
};

