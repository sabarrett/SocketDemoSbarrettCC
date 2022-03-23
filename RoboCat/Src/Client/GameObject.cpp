#include "Client/GameObject.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

void GameObject::Init(std::string name, int UID)
{
	m_name = name;
	m_UID = UID;
}

void GameObject::InitRenderer(Game* game, std::string filePath)
{
	m_gameRenderer = game->getRenderer();
	SDL_Surface* tmpSurface = IMG_Load(filePath.c_str());
	printf(" %s\n", SDL_GetError());
	m_texture = SDL_CreateTextureFromSurface(m_gameRenderer, tmpSurface);
	
	m_textureRect = tmpSurface->clip_rect;
	m_textureRect.h *= 0.12f;
	m_textureRect.w *= 0.12f;
	SDL_FreeSurface(tmpSurface);
}

void GameObject::Update(const Uint8* m_keyStates)
{
	bool moving = false;
	float yDeltaVel = 0;
	float xDeltaVel = 0;

	if (m_keyStates[SDL_SCANCODE_W])
	{
		yDeltaVel += -2;
		moving = true;
	}
	if (m_keyStates[SDL_SCANCODE_S])
	{
		yDeltaVel += 2;
		moving = true;
	}
	if (m_keyStates[SDL_SCANCODE_A])
	{
		xDeltaVel += -2;
		moving = true;
	}
	if (m_keyStates[SDL_SCANCODE_D])
	{
		xDeltaVel += 2;
		moving = true;
	}

	float mag = sqrt(((double)xDeltaVel * (double)xDeltaVel) + ((double)yDeltaVel * (double)yDeltaVel));
	if (mag > 0)
	{
		yDeltaVel /= mag;
		xDeltaVel /= mag;
	}

	m_xVel += xDeltaVel;
	m_yVel += yDeltaVel;

	if (m_xVel != 0 && !moving)
	{
		m_xVel *= 0.8;
	}
	if (m_yVel != 0 && !moving)
	{
		m_yVel *= 0.8;
	}

	m_xPos += m_xVel;

	m_yPos += m_yVel;
}

void GameObject::Draw()
{
	SDL_Rect* dstRect = &m_textureRect;
	dstRect->x = m_xPos;
	dstRect->y = m_yPos;
	SDL_RenderCopy(m_gameRenderer, m_texture, NULL, dstRect);
}

void GameObject::CleanUp()
{

}


