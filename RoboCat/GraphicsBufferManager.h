#pragma once
#include<iostream>
#include"../common/DeanLib/include/Trackable.h"
#include<map>
#include<iterator>

/*
Author: Nicholas Preis
	Class : Game Architecture <250-71>
	Assignment : Assignment 4
	Certification of Authenticity :
I certify that this assignment is entirely my own work.
*/

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

