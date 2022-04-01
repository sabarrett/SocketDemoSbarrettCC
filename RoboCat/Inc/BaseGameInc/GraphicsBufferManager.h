#pragma once

#include <Trackable.h>
#include <map>
#include <string>

#include "Color.h"

class GraphicsBuffer;

typedef std::string GBKey;

class GraphicsBufferManager :public Trackable
{
public:
	GraphicsBufferManager();
	~GraphicsBufferManager();

	void clear();

	const GraphicsBuffer* createBuffer(const GBKey& key, const std::string& filename);
	const GraphicsBuffer* createBuffer(const GBKey& key, unsigned int width, unsigned int height, Color color = Color());
	const GraphicsBuffer* cloneBuffer(const GBKey& newKey, const GBKey& existingKey);//make a copy of existingKey's buffer and call it newKey
	const GraphicsBuffer* getBuffer(const GBKey& key) const;
	bool destroyBuffer(const GBKey& key);
	bool destroyBuffer(GraphicsBuffer* pBuffer);

	unsigned int getNumBuffersManaged() const { return mMap.size(); };
	bool alreadyExists(const GBKey& key) const;

private:
	std::map<GBKey, GraphicsBuffer*> mMap;
};
