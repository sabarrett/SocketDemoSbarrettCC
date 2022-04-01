#pragma once

#include <Trackable.h>
#include <Vector2D.h>

#include <allegro5/allegro.h>

#include "Color.h"
#include "Font.h"

class GraphicsBuffer;
class Sprite;
class System;

class GraphicsSystem :public Trackable
{
	friend class System;
public:
	unsigned int getDisplayWidth() const { return mWidth; };
	unsigned int getDisplayHeight() const { return mHeight; };
	GraphicsBuffer* getBackBuffer() const { return mpBackBuffer; };

	void flip();

	static void draw(const Vector2D& destLoc, const GraphicsBuffer& src, double scale = 1.0);
	static void draw(GraphicsBuffer& dest, const Vector2D& loc, const GraphicsBuffer& src, double scale = 1.0);
	static void draw(const Vector2D& destLoc, const Sprite& sprite, double scale = 1.0);
	static void draw(GraphicsBuffer& dest, const Vector2D& destLoc, const Sprite& sprite, double scale = 1.0);
	static void writeText(GraphicsBuffer& dest, const Vector2D& destLoc, const Font& font, const Color& color, const std::string& text, Font::Alignment align = Font::LEFT);
	static void writeText(const Vector2D& destLoc, const Font& font, const Color& color, const std::string& text, Font::Alignment align = Font::LEFT);

	static void setBufferToColor(GraphicsBuffer& buffer, const Color& color);
	static void saveBufferToFile(const GraphicsBuffer& buffer, const std::string& filename);
private:
	ALLEGRO_DISPLAY* mDisplay = NULL;
	GraphicsBuffer* mpBackBuffer = NULL;
	bool mIsInitted = false;
	unsigned int mWidth = 0;
	unsigned int mHeight = 0;

	GraphicsSystem();
	~GraphicsSystem();

	bool init(unsigned int width, unsigned int height);
	void cleanup();

	bool initAddOns();
	ALLEGRO_DISPLAY* getDisplay() const { return mDisplay; };
	static ALLEGRO_COLOR getAllegroColorFromColor(const Color& color);
	static int getAllegroFontAlignFlag(Font::Alignment align);


};
