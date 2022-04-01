#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <Trackable.h>

#include <string>

class GraphicsSystem;

class Font:public Trackable
{
	friend class GraphicsSystem;
public:
	enum Alignment
	{
		LEFT,
		CENTER,
		RIGHT
	};

	Font(const std::string& filename,int size);
	~Font();

	int getSize() const { return mSize; };

private:
	ALLEGRO_FONT * mpFont;
	int mSize;

	ALLEGRO_FONT* getFont() const { return mpFont; };
};
