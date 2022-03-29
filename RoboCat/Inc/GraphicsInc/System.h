#pragma once

#include <Trackable.h>
#include <Vector2D.h>

#include <allegro5/allegro.h>

class GraphicsSystem;

class System :public Trackable
{
public:
	enum MouseButton
	{
		LEFT,
		RIGHT,
		CENTER
	};

	enum Key
	{
		F_KEY = ALLEGRO_KEY_F,
		S_KEY = ALLEGRO_KEY_S,
		ENTER_KEY = ALLEGRO_KEY_ENTER,
		SPACE_KEY = ALLEGRO_KEY_SPACE,
		ESCAPE_KEY = ALLEGRO_KEY_ESCAPE,
		ONE_KEY = ALLEGRO_KEY_1,
		TWO_KEY = ALLEGRO_KEY_2
	};

	System();
	virtual ~System();

	bool init(unsigned int width, unsigned int height);
	void cleanup();
	GraphicsSystem* getGraphicsSystem() const { return mpGraphicsSystem; };

	Vector2D getCurrentMousePos();
	bool isMouseButtonPressed(System::MouseButton button);
	bool isKeyPressed(System::Key theKey);
private:
	bool mIsInitted = false;
	GraphicsSystem* mpGraphicsSystem = NULL;

};
