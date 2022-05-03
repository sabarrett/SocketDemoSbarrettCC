#include "GraphicsBufferManager.h"
#include "GraphicsBuffer.h"

using namespace std;

GraphicsBufferManager::GraphicsBufferManager()
{

}

GraphicsBufferManager::~GraphicsBufferManager()
{
    if(mGraphicsBuffers.size() > 0)
        clear();
}

GraphicsBuffer* GraphicsBufferManager::createGraphicsBuffer(string filename)
{
    return new GraphicsBuffer(filename);
}

GraphicsBuffer* GraphicsBufferManager::createAndManageGraphicsBuffer(string key, string filename)
{
    GraphicsBuffer* gb = new GraphicsBuffer(filename);
    mGraphicsBuffers.emplace(key, gb);
    return gb;
}

void GraphicsBufferManager::addGraphicsBuffer(string key, GraphicsBuffer* gb)
{
    mGraphicsBuffers.emplace(key, gb);
}

void GraphicsBufferManager::removeGraphicsBuffer(string key)
{
    mGraphicsBuffers.erase(key);
}

void GraphicsBufferManager::removeAndDeleteGraphicsBuffer(string key)
{
    delete mGraphicsBuffers.at(key);
    mGraphicsBuffers.erase(key);
}

void GraphicsBufferManager::deleteGraphicsBuffer(GraphicsBuffer* gb)
{
    delete gb;
}

string GraphicsBufferManager::findGraphicsBufferKey(GraphicsBuffer* gb)
{
    for(unordered_map<string, GraphicsBuffer*>::iterator i = mGraphicsBuffers.begin(); i != mGraphicsBuffers.end(); i++)
    {
        if((*i).second == gb)
            return (*i).first;
    }
    return "";
}

GraphicsBuffer* GraphicsBufferManager::getGraphicsBuffer(string key)
{
    return mGraphicsBuffers.at(key);
}

void GraphicsBufferManager::clear()
{
    for(unordered_map<string, GraphicsBuffer*>::iterator i = mGraphicsBuffers.begin(); i != mGraphicsBuffers.end(); i++)
    {
        delete (*i).second;
    }
    mGraphicsBuffers.clear();
}