#include "RoboCatPCH.h"
#include "..\GameFiles\GameObject.h"
#include "allegro_wrapper_functions-main/GraphicsLibrary.h"

GameObject::GameObject(std::string graphicsID, int posX, int posY, int width, int height)
{
	mpGraphicsID = new std::string(graphicsID);
	mGraphicsData.mX = posX;
	mGraphicsData.mY = posY;
	mGraphicsData.mZ = width;
	mGraphicsData.mW = height;
}

GameObject::GameObject(std::string graphicsID, int width, int height)
{
	mpGraphicsID = new std::string(graphicsID);
	mGraphicsData.mX = 0;
	mGraphicsData.mY = 0;
	mGraphicsData.mZ = width;
	mGraphicsData.mW = height;
}

void GameObject::Write(OutputMemoryBitStream& stream)
{
	stream.Write(mGraphicsData.mX);
	stream.Write(mGraphicsData.mY);
	stream.Write(mGraphicsData.mZ);
	stream.Write(mGraphicsData.mW);
	stream.Write(*mpGraphicsID);
}

void GameObject::Read(InputMemoryBitStream& stream)
{
	stream.Read(mGraphicsData.mX);
	stream.Read(mGraphicsData.mY);
	stream.Read(mGraphicsData.mZ);
	stream.Read(mGraphicsData.mW);
	stream.Read(*mpGraphicsID);
}

void GameObject::Render(GraphicsLibrary* gl)
{
	gl->drawImage(*mpGraphicsID, mGraphicsData.mX - (mGraphicsData.mZ / 2.0f), mGraphicsData.mY - (mGraphicsData.mW / 2.0f));
}

void GameObject::SetPostion(int posX, int posY)
{
	mGraphicsData.mX = posX;
	mGraphicsData.mY = posY;
}

Vector3 GameObject::GetPosition()
{
	return Vector3(mGraphicsData.mX,mGraphicsData.mY,0);
}

GameObject::~GameObject()
{
	delete mpGraphicsID;
}
