#include "AnimationData.h"
#include "Sprite.h"
#include "GraphicsBuffer.h"

AnimationData::AnimationData(Sprite* frames, int numOfFrames)
{
	mSprites.resize(numOfFrames);

	for(int i = 0; i < numOfFrames; i++)
	{
		mSprites.push_back(frames + i); //Pointer math
	}

	mOwnsSprites = false;
}

AnimationData::AnimationData(GraphicsBuffer* gb, int rows, int columns, float scale)
{
	mOwnsSprites = true;

	int spriteWidth = gb->getWidth() / columns;
	int spriteHeight = gb->getHeight() / rows;

	
	for(int j = 0; j < rows; j++)
	{
		for(int i = 0; i < columns; i++)
		{
			Sprite* spr = new Sprite(gb, Vector2D(i * spriteWidth, j * spriteHeight), Vector2D(spriteWidth, spriteHeight), scale);
			mSprites.push_back(spr);
		}
	}
}

AnimationData::~AnimationData()
{
	if (mOwnsSprites)
	{
		while(mSprites.size() > 0)
		{
			Sprite* spr = mSprites.back();
			delete spr;
			mSprites.pop_back();
		}	
		
	}
}

Sprite* AnimationData::getSprite(int index)
{
    return mSprites.at(index);
}