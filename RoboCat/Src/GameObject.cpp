#include "GameObject.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

void GameObject::Init(SDL_Renderer* gameRenderer, std::string name)
{
	m_gameRenderer = gameRenderer;

	m_name = name;

	SDL_Surface* tmpSurface = IMG_Load("Assets/Player.png");
	m_texture = SDL_CreateTextureFromSurface(m_gameRenderer, tmpSurface);
	m_textureRect = tmpSurface->clip_rect;
	SDL_FreeSurface(tmpSurface);
}

void GameObject::Init(SDL_Renderer* gameRenderer, std::string name, std::string filePath)
{
	m_gameRenderer = gameRenderer;

	m_name = name;

	SDL_Surface* tmpSurface = IMG_Load(filePath.c_str());
	m_texture = SDL_CreateTextureFromSurface(m_gameRenderer, tmpSurface);
	m_textureRect = tmpSurface->clip_rect;
	SDL_FreeSurface(tmpSurface);
}

void GameObject::Update(Game* game)
{
	if (game->getKeyStates()[SDL_SCANCODE_W])
	{
		m_yVel += -2;
	}
	if (game->getKeyStates()[SDL_SCANCODE_S])
	{
		m_yVel += 2;
	}
	if (game->getKeyStates()[SDL_SCANCODE_A])
	{
		m_xVel += -2;
	}
	if (game->getKeyStates()[SDL_SCANCODE_D])
	{
		m_xVel += 2;
	}

	if (m_xVel != 0)
	{
		m_xVel *= 0.8;
	}
	if (m_yVel != 0)
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


