#include"GraphicsBufferManager.h"
#include"GraphicsBuffer.h"

/*
Author: Nicholas Preis
	Class : Game Architecture <250-71>
	Assignment : Assignment 4
	Certification of Authenticity :
I certify that this assignment is entirely my own work.
*/

GraphicsBufferManager::GraphicsBufferManager()
{
	mDeleted = false;
	mBuffer = new map<string, GraphicsBuffer*>();
}

GraphicsBufferManager::~GraphicsBufferManager()
{
	cleanup();
}

void GraphicsBufferManager::addBuffer(string bufferName, GraphicsBuffer* buffer)
{
	mBuffer->emplace(bufferName, buffer);
}

void GraphicsBufferManager::deleteBuffer(string bufferName)
{
	delete mBuffer->find(bufferName)->second;
	mBuffer->erase(bufferName);
}

GraphicsBuffer* GraphicsBufferManager::getGraphicsBuffer(string bufferName)
{
	map<string, GraphicsBuffer*>::iterator iter;
	iter = mBuffer->find(bufferName);

	if (iter != mBuffer->end())
	{
		return iter->second;
	}

	return NULL;
}

void GraphicsBufferManager::cleanup()
{
	if (!mDeleted)
	{
		map<string, GraphicsBuffer*>::iterator iterate;
		for (iterate = mBuffer->begin(); iterate != mBuffer->end(); iterate++)
		{
			iterate->second;
		}

		mDeleted = true;
	}
}
