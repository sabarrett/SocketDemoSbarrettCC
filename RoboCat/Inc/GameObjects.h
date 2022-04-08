#pragma once
#include <GraphicsSystems.h>

#include <vector>
#include <string>
#include <map>
#include <utility>

using std::vector;
using std::pair;
using std::string;
using std::map;

enum ClassId
{
	DEFAULT = 'AAAA',
	DEANSPRITE = 'RJGH',
	AMONGUS = 'JJGD',
	SCOTTSPRITE = 'LJDE',
};

class GameObject
{
public:

	GameObject() {};
	virtual ~GameObject();
	virtual void Draw();

	int getNetworkId() { return mNetworkId; };
	void setNetworkId(int newNetworkId) { mNetworkId = newNetworkId; };

	std::pair<float, float> getPosition() { return mPosition; };
	void setPosition(float positionX, float positionY) { mPosition.first = positionX; mPosition.second = positionY; };

	ClassId getClassId() { return mId; };
	GraphicsSystems* getGraphics() { return mGraphics; };

	ALLEGRO_BITMAP* getmpBitmap() { return mpBitmap; };
protected:
	GameObject(int networkId, std::pair<float, float> position, GraphicsSystems* graphics, std::string imageFilePath, ClassId id = DEFAULT);
private:
	int mNetworkId;
	std::pair<float, float> mPosition;
	ClassId mId;
	GraphicsSystems* mGraphics;
	ALLEGRO_BITMAP* mpBitmap;

};

class DeanSprite : public GameObject
{
public:
	DeanSprite(int networkId, std::pair<float, float> position, std::string imageFilePath, GraphicsSystems* graphics);
};

class AmongUs : public GameObject
{
public:
	AmongUs(int networkId, std::pair<float, float> position, std::string imageFilePath, GraphicsSystems* graphics);
};

class ScottSprite : public GameObject
{
public:
	ScottSprite(int networkId, std::pair<float, float> position, std::string imageFilePath, GraphicsSystems* graphics);
};