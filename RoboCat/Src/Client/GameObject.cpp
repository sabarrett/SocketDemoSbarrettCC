#include "Client/GameObject.h"
#include "Util/RoboCatPCH.h"


void GameObject::Init(int UID, uint8_t textureID)
{
	m_UID = UID;
	m_textureID = textureID;
	m_containsUpdatedInfo = false;
	m_objectType = GAMEOBJECT_BASE;
}

void GameObject::Update(std::vector<GameObject*> collidableObjects)
{
	if (abs(m_xVel) > 0.01 || abs(m_yVel) > 0.01)
	{
		// Drag
		m_xVel *= 0.8;
		m_yVel *= 0.8;

		// Update Position
		SetPosition(m_xPos + m_xVel, m_yPos + m_yVel);
	}
}

void GameObject::AddVelocityFromInput(uint8_t moveInput)
{
	float xDeltaVel = 0;
	float yDeltaVel = 0;
	if (moveInput & MOVE_UP)
	{
		yDeltaVel += -2;
	}
	if (moveInput & MOVE_DOWN)
	{
		yDeltaVel += 2;
	}
	if (moveInput & MOVE_LEFT)
	{
		xDeltaVel += -2;
	}
	if (moveInput & MOVE_RIGHT)
	{
		xDeltaVel += 2;
	}
	float mag = sqrt((xDeltaVel * xDeltaVel) + (yDeltaVel * yDeltaVel));
	if (mag > 0)
	{
		yDeltaVel /= mag;
		xDeltaVel /= mag;
	}
	m_xVel += xDeltaVel;
	m_yVel += yDeltaVel;
}

void GameObject::Draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
	SDL_Rect dstRect;
	dstRect.x = (int)m_xPos-16;
	dstRect.y = (int)m_yPos-16;
	dstRect.h = 32;
	dstRect.w = 32;
	SDL_RenderCopy(renderer, texture, NULL, &dstRect);
}

void GameObject::CleanUp()
{

}

void GameObject::Write(OutputMemoryBitStream* outStream)
{
	if (m_containsUpdatedInfo)
	{
		outStream->Write(m_textureID);
		outStream->Write(m_xPos);
		outStream->Write(m_yPos);
		m_containsUpdatedInfo = false;
	}
}

void GameObject::Read(InputMemoryBitStream* inStream)
{
	bool readMore;
	inStream->Read(readMore);
	if (readMore)
	{
		inStream->Read(m_textureID);
		inStream->Read(m_xPos);
		inStream->Read(m_yPos);
	}
}


