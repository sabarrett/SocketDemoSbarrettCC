#include "GraphicsBufferManager.h"
#include "GraphicsBuffer.h"

#include "MemoryManager.h"

#include "Game.h"

using namespace std;

GraphicsBufferManager::GraphicsBufferManager()
{
}

GraphicsBufferManager::~GraphicsBufferManager()
{
	clear();
}

void GraphicsBufferManager::clear()
{
	map<GBKey, GraphicsBuffer*>::iterator iter;
	MemoryManager* pMemoryManager = Game::getInstance()->getMemoryManager();
	for (iter = mMap.begin(); iter != mMap.end(); ++iter)
	{
		iter->second->~GraphicsBuffer();
		pMemoryManager->deallocate((Byte*)iter->second);
	}

	mMap.clear();
}

const GraphicsBuffer* GraphicsBufferManager::createBuffer(const GBKey& key, const std::string& filename)
{
	GraphicsBuffer* pBuffer = NULL;
	MemoryManager* pMemoryManager = Game::getInstance()->getMemoryManager();
	Byte* ptr = pMemoryManager->allocate(sizeof(GraphicsBuffer));

	if (!alreadyExists(key))
	{
		GraphicsBuffer* pBuffer = new(ptr)GraphicsBuffer(filename);
		mMap[key] = pBuffer;
	}

	return pBuffer;
}

const GraphicsBuffer* GraphicsBufferManager::createBuffer(const GBKey& key, unsigned int width, unsigned int height, Color color /*= Color()*/)
{
	GraphicsBuffer* pBuffer = NULL;
	MemoryManager* pMemoryManager = Game::getInstance()->getMemoryManager();
	Byte* ptr = pMemoryManager->allocate(sizeof(GraphicsBuffer));

	if (!alreadyExists(key))
	{
		GraphicsBuffer* pBuffer = new(ptr)GraphicsBuffer( width, height, color );
		mMap[key] = pBuffer;
	}

	return pBuffer;
}

const GraphicsBuffer* GraphicsBufferManager::cloneBuffer(const GBKey& newKey, const GBKey& existingKey)
{
	GraphicsBuffer* pNewBuffer = NULL;
	const GraphicsBuffer* pExistingBuffer = getBuffer(existingKey);



	if (!alreadyExists(newKey) && pExistingBuffer != NULL)
	{
		pNewBuffer = pExistingBuffer->clone();
		mMap[newKey] = pNewBuffer;
	}

	return pNewBuffer;
}

const GraphicsBuffer* GraphicsBufferManager::getBuffer(const GBKey& key) const
{
	map<GBKey, GraphicsBuffer*>::const_iterator iter = mMap.find(key);

	if (iter != mMap.end())
	{
		return iter->second;
	}
	else
	{
		return NULL;
	}
}

bool GraphicsBufferManager::destroyBuffer(const GBKey& key)
{
	if (alreadyExists(key))
	{
		MemoryManager* pMemoryManager = Game::getInstance()->getMemoryManager();
		const GraphicsBuffer* pBuffer = getBuffer(key);
		pBuffer->~GraphicsBuffer();
		pMemoryManager->deallocate((Byte*)pBuffer);
		mMap.erase(key);
		return true;
	}

	return false;
}

bool GraphicsBufferManager::destroyBuffer(GraphicsBuffer* pBuffer)
{
	map<GBKey, GraphicsBuffer*>::const_iterator iter;
	MemoryManager* pMemoryManager = Game::getInstance()->getMemoryManager();
	for (iter = mMap.begin(); iter != mMap.end(); ++iter)
	{
		if (pBuffer == iter->second)
		{
			pBuffer->~GraphicsBuffer();
			pMemoryManager->deallocate((Byte*)pBuffer);
			mMap.erase(iter);
			return true;
		}
	}

	return false;
}

bool GraphicsBufferManager::alreadyExists(const GBKey& key) const
{
	if (getBuffer(key) == NULL)
	{
		return false;
	}
	else
	{
		return true;
	}
}
