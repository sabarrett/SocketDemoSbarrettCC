#include "GraphicsBufferManager.h"

GraphicsBufferManager::GraphicsBufferManager()
{
}

GraphicsBufferManager::~GraphicsBufferManager()
{
}

void GraphicsBufferManager::cleanup()
{
	clear();
}

void GraphicsBufferManager::addGBuffer(string key, GraphicsBuffer* newBuffer)
{
	mUMap[key] = newBuffer;
}

void GraphicsBufferManager::addGBuffer(string key, string ASSET_PATH)
{
	mUMap[key] = new GraphicsBuffer(ASSET_PATH);
}

void GraphicsBufferManager::deleteGBuffer(GraphicsBuffer* bufferToDelete)
{
	//run through map until buffer is found
	for (auto it = mUMap.begin(); it != mUMap.end(); it++)
	{
		if (it->second == bufferToDelete)
		{
			mUMap.erase(it);
		}
	}
}

void GraphicsBufferManager::deleteGBuffer(string key)
{
	mUMap.erase(key);
}

GraphicsBuffer* GraphicsBufferManager::getGBuffer(string key)
{
	return mUMap.find(key)->second;
}

void GraphicsBufferManager::clear()
{
	//run through map and delete all buffers
	for (auto it = mUMap.begin(); it != mUMap.end(); it++)
	{
		delete (it->second);
	}

	mUMap.clear();
}

int GraphicsBufferManager::getBHeight(string key)
{
	return mUMap.find(key)->second->getHeight();
}

int GraphicsBufferManager::getBWidth(string key)
{
	return mUMap.find(key)->second->getWidth();
}
