#pragma once

using namespace std;

#include "Trackable.h"
#include <unordered_map>
#include "GraphicsBuffer.h"

class GraphicsBufferManager : public Trackable
{
private:
	unordered_map<string, GraphicsBuffer*> mUMap;

public:
	GraphicsBufferManager();
	~GraphicsBufferManager();

	void cleanup();

	//ADDERS
	void addGBuffer(string key, GraphicsBuffer* newBuffer);
	void addGBuffer(string key,string ASSET_PATH);

	//DELETERS
	void deleteGBuffer(GraphicsBuffer* bufferToDelete);
	void deleteGBuffer(string key);
	void clear();

	//GETTERS
	GraphicsBuffer* getGBuffer(string key);
	int getBHeight(string key);
	int getBWidth(string key);
};