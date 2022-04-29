#include "WidgetManager.h"
#include "Widget.h"

using namespace std;

WidgetManager::~WidgetManager()
{
	cleanup();
}

void WidgetManager::cleanup()
{
	//go through all entries in map and delete
	for( auto iter : mMap )
	{
		Widget* pWidget = iter.second;
		delete pWidget;
	}

	mMap.clear();
}

Widget* WidgetManager::createAndManageWidget( const WidgetKey& key, float contents )
{
	Widget* pWidget = NULL;

	//figure out if it exists already
	auto iter = mMap. find( key );

	//already in map?
	if( iter == mMap.end() )
	{
		//not already there - just create and add it
		pWidget = new Widget( contents );
		mMap[key] = pWidget;
	}

	return pWidget;
}

void WidgetManager::deleteWidget( const WidgetKey& key )
{
	//figure out if it exists already
	auto iter = mMap.find( key );

	if( iter != mMap.end() )//found
	{
		delete iter->second;
		mMap.erase( iter );
	}

}

void WidgetManager::deleteWidget( Widget* pWidget )
{
	//go through all entries in map searching for pWidget
	for( auto iter: mMap )
	{
		if( pWidget == iter.second )
		{
			delete pWidget;
			mMap.erase( iter.first );
			return;
		}
	}
	
}

Widget* WidgetManager::getWidget( const WidgetKey& key ) const
{
	auto iter = mMap.find( key );
	
	if( iter != mMap.end() )
	{
		return iter->second;
	}
	else
	{
		return NULL;
	}
}
