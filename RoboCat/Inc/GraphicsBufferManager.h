#pragma once
#include "GraphicsBuffer.h"
#include <map>

typedef std::string BufferKey;

class GraphicsBufferManager
{
public:
	GraphicsBufferManager();
	~GraphicsBufferManager();

	void addBuffer();
	void deleteBuffer();
	void clear();
	GraphicsBuffer* getBufferAt(BufferKey key);
};