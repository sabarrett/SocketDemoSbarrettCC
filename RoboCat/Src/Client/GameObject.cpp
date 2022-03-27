#include "Client/GameObject.h"
#include "Util/RoboCatPCH.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

void GameObject::Init(int UID, uint8_t textureID)
{
	m_UID = UID;
	m_textureID = textureID;
	containsUpdatedInfo = false;
	objectType = 0;
}

void GameObject::Update(std::vector<GameObject*> collidableObjects)
{
	if (m_xVel != 0)// && !moving)
	{
		m_xVel *= 0.8;
	}
	if (m_yVel != 0)// && !moving)
	{
		m_yVel *= 0.8;
	}
	if (m_xVel > 0.01  || m_xVel < -0.01 ||
		m_yVel > 0.01 || m_yVel < -0.01)
	{
		SetPosition(m_xPos + m_xVel, m_yPos + m_yVel);
	}
}

void GameObject::AddVelocityFromInput(int moveInput)
{
	float yDeltaVel = 0;
	float xDeltaVel = 0;
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
	float mag = sqrt(((double)xDeltaVel * (double)xDeltaVel) + ((double)yDeltaVel * (double)yDeltaVel));
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
	//SDL_QueryTexture(texture, NULL, NULL, &dstRect.w, &dstRect.h);
	dstRect.x = (int)m_xPos;
	dstRect.y = (int)m_yPos;
	dstRect.h = 32;
	dstRect.w = 32;
	SDL_RenderCopy(renderer, texture, NULL, &dstRect);
}

void GameObject::CleanUp()
{

}

void GameObject::Write(OutputMemoryBitStream* outStream)
{
	outStream->Write(objectType);
	outStream->Write(containsUpdatedInfo);
	if (containsUpdatedInfo)
	{
		outStream->Write(m_textureID);
		/*outStream->Write(ConvertToFixed(m_xPos, 0, 3));
		outStream->Write(ConvertToFixed(m_yPos, 0, 3));*/
		outStream->Write(m_xPos);
		outStream->Write(m_yPos);
		containsUpdatedInfo = false;
	}
}

void GameObject::Read(InputMemoryBitStream* inStream)
{
	bool readMore;
	//inStream->ReadBits(&readMore, 1);
	inStream->Read(readMore);
	if (readMore)
	{
		inStream->Read(m_textureID);
		/*uint32_t x;
		uint32_t y;
		inStream->Read(x);
		inStream->Read(y);
		m_xPos = ConvertFromFixed(x, 0, 3);
		m_yPos = ConvertFromFixed(y, 0, 3);*/
		inStream->Read(m_xPos);
		inStream->Read(m_yPos);
	}
}


