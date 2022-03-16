#include "DataRepository.h"
#include <cstring>

DataEntry::DataEntry( int val )
	:mType(INT_VAL)
{
	mData.intVal = val;
}

DataEntry::DataEntry( float val )
	:mType(FLOAT_VAL)
{
	mData.floatVal = val;
}

DataEntry::DataEntry( double val )
	:mType(DOUBLE_VAL)
{
	mData.doubleVal = val;
}

DataEntry::DataEntry( const std::string& val )
	:mType(STRING_VAL)
{
	assert( val.size() + 1 < MAX_STRING_VAL_SIZE );
	strcpy_s( mData.stringVal, val.c_str() );
}

DataEntry::DataEntry( UINT val )
	:mType(UINT_VAL)
{
	mData.uintVal = val;
}

void DataRepository::addEntry( const DataKey& key, int val )
{
	DataEntry entry( val );
	mMap[key] = entry;
}

void DataRepository::addEntry( const DataKey& key, float val )
{
	DataEntry entry( val );
	mMap[key] = entry;
}

void DataRepository::addEntry( const DataKey& key, double val )
{
	DataEntry entry( val );
	mMap[key] = entry;
}

void DataRepository::addEntry( const DataKey& key, const std::string& val )
{
	DataEntry entry( val );
	mMap[key] = entry;
}

void DataRepository::addEntry( const DataKey& key, UINT val )
{
	DataEntry entry( val );
	mMap[key] = entry;
}

const DataEntry& DataRepository::getEntry( const DataKey& key )
{
	static DataEntry nullEntry(0);

	auto iter = mMap.find( key );
	if( iter != mMap.end() )
	{
		return iter->second;
	}
	else
	{
		return nullEntry;
	}
}

bool DataRepository::hasEntry(const DataKey& key)
{
	auto iter = mMap.find(key);
	if (iter != mMap.end())
		return true;
	else
		return false;
}

