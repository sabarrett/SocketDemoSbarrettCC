#pragma once
#include<iostream>
#include"../common/DeanLib/include/Trackable.h"
#include<map>
#include<iterator>

using namespace std;

class GraphicsBuffer;
class Color;

class GraphicsBufferManager
{
public:
	GraphicsBufferManager();
	~GraphicsBufferManager();

	void addBuffer(string bufferName, GraphicsBuffer* buffer);
	void deleteBuffer(string bufferName);
	GraphicsBuffer* getGraphicsBuffer(string bufferName);

	void cleanup();
private:
	map<string, GraphicsBuffer*>* mBuffer;
	bool mDeleted;
};

