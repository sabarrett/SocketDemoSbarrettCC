#include "RoboCatPCH.h"
#include "Unit.h"

void Unit::update(float dt, Vector2 screen)
{
	int gravity = 4;

	switch (type)
	{
	case 0:
		// square - cycle through colors
		color.incrementColor();
		break;

	case 1:
		// vRect - fall
		if (position.y >= screen.y)
		{
			position.y = -size.y;
		}
		position.y += gravity;
		rect.y = position.y;
		break;

	case 2:
		// hRect - teleport
		if (position.x <= 0)
			right = true;
		else if (position.x + size.x >= screen.x)
			right = false;

		if (right)
			position.x += 3;
		else
			position.x -= 3;

		rect.x = position.x;

		break;
	}
}

void Unit::render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	// draw rect
	SDL_RenderFillRect(renderer, &rect);

	SDL_RenderPresent(renderer);
}