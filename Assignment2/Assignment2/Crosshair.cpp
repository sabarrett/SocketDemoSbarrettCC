#include "Crosshair.h"

Crosshair::Crosshair()
{
	mPosition = Vec2D(100, 100);
	mSize = Vec2D(32, 32);
}

void Crosshair::handleEvent(SDL_Event& e)
{
	int mouseX, mouseY;

	if (e.type == SDL_MOUSEMOTION)
	{
		SDL_GetMouseState(&mouseX, &mouseY);

		mPosition = Vec2D(mouseX, mouseY);
	}
}

void Crosshair::setColor(Color newColor)
{
	mColor = newColor;
}

void Crosshair::update()
{
}

void Crosshair::render()
{
	SDL_SetTextureColorMod(crosshairTexture.getTexture(), mColor.r, mColor.g, mColor.b);
	crosshairTexture.render(mPosition.x, mPosition.y);
}

Texture Crosshair::crosshairTexture = Texture();