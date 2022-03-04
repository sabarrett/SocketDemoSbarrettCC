#include "GraphicsBuffer.h"

using namespace std;

GraphicsBuffer::GraphicsBuffer(string filename)
{
	mTexture = new raylib::Texture(filename);
	mWidth = mTexture->GetWidth();
	mHeight = mTexture->GetHeight();
}

GraphicsBuffer::~GraphicsBuffer()
{
	unload();
	delete mTexture;
	mTexture = nullptr;
}

void GraphicsBuffer::loadFromFile(string filename)
{
	unload();
	mTexture->Load(filename);
	mWidth = mTexture->GetWidth();
	mHeight = mTexture->GetHeight();
}

void GraphicsBuffer::unload()
{
	mTexture->Unload();
	mWidth = 0;
	mHeight = 0;
}