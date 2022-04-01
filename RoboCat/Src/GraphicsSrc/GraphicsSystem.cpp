#include "GraphicsSystem.h"
#include "GraphicsBuffer.h"
#include "Sprite.h"
#include "Color.h"

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include <iostream>
#include <cassert>

using namespace std;


GraphicsSystem::GraphicsSystem()
	:mDisplay(NULL)
	,mIsInitted(false)
{
}

GraphicsSystem::~GraphicsSystem()
{
	cleanup();
}

bool GraphicsSystem::init(unsigned int width, unsigned int height)
{
	//possible enhancement - if width and height is different than current initted display - cleanup and re-init with new size
	if (!mIsInitted)
	{
		if (!initAddOns())
		{
			cout << "error initting Allegro Add-Ons\n";
			return false;
		}
		mDisplay = al_create_display(width, height);
		mWidth = al_get_display_width(mDisplay);
		mHeight = al_get_display_height(mDisplay);
		mpBackBuffer = new GraphicsBuffer(mDisplay);
		mIsInitted = true;
	}
	return true;
}

void GraphicsSystem::cleanup()
{
	if (mIsInitted)
	{
		delete mpBackBuffer;
		mpBackBuffer = NULL;

		al_uninstall_audio();
		al_shutdown_primitives_addon();
		al_shutdown_ttf_addon();
		al_shutdown_font_addon();
		al_shutdown_image_addon();
		al_destroy_display(mDisplay);
		mIsInitted = false;
		mDisplay = NULL;
	}
}

bool GraphicsSystem::initAddOns()
{
	if (!al_init_image_addon())
	{
		cout << "error - Image Add-on not initted\n";
		return false;
	}
	if (!al_init_font_addon())
	{
		cout << "error - Font Add-on not initted\n";
		return false;
	}
	if (!al_init_ttf_addon())
	{
		cout << "error - TTF Add-on not initted\n";
		return false;
	}
	if (!al_init_primitives_addon())
	{
		cout << "error - primitives Add-on not initted\n";
		return false;
	}
	if (!al_install_keyboard())
	{
		cout << "error - keyboard not installed\n";
		return false;
	}
	if (!al_install_mouse())
	{
		cout << "error - mouse not installed\n";
		return false;
	}

	if (!al_install_audio())
	{
		cout << "Warning!!! - Audio Add-on not initted\n";
	}
	else if (!al_init_acodec_addon())
	{
		cout << "Warning!!! - Audio Codec Add-on not initted\n";
	}
	else if (!al_reserve_samples(1))
	{
		cout << "Warning!!! - samples not reserved\n";
	}


	return true;
}

void GraphicsSystem::flip()
{
	assert(mIsInitted);
	al_flip_display();
}

void GraphicsSystem::draw(const Vector2D& destLoc, const GraphicsBuffer& src, double scale /*= 1.0*/)
{
	ALLEGRO_BITMAP* pTarget = al_get_target_bitmap();
	assert(pTarget != src.getBitmap());

	if (pTarget != src.getBitmap())
	{
		int srcW = src.getWidth();
		int srcH = src.getHeight();

		al_draw_scaled_bitmap(src.getBitmap(), 0.0f, 0.0f, srcW, srcH, destLoc.getX(), destLoc.getY(), srcW*scale, srcH*scale, 0);
	}
}

void GraphicsSystem::draw(GraphicsBuffer& dest, const Vector2D& loc, const GraphicsBuffer& src, double scale /*= 1.0*/)
{
	assert(dest.getBitmap() != src.getBitmap());
	if (dest.getBitmap() != src.getBitmap())
	{
		ALLEGRO_BITMAP* pOldTarget = al_get_target_bitmap();
		al_set_target_bitmap(dest.getBitmap());
		draw(loc, src, scale);
		al_set_target_bitmap(pOldTarget);
	}
}

void GraphicsSystem::draw(const Vector2D& destLoc, const Sprite& sprite, double scale /*= 1.0*/)
{
	ALLEGRO_BITMAP* pTarget = al_get_target_bitmap();
	const GraphicsBuffer* pSrc = sprite.getBuffer();
	assert(pTarget != sprite.getBuffer()->getBitmap());

	if (pTarget != sprite.getBuffer()->getBitmap())
	{
		int srcW = sprite.getWidth();
		int srcH = sprite.getHeight();

		al_draw_scaled_bitmap(pSrc->getBitmap(), sprite.getSourceLoc().getX(), sprite.getSourceLoc().getY(), srcW, srcH, destLoc.getX(), destLoc.getY(), srcW*scale, srcH*scale, 0);
	}

}

void GraphicsSystem::draw(GraphicsBuffer& dest, const Vector2D& destLoc, const Sprite& sprite, double scale /*= 1.0*/)
{
	ALLEGRO_BITMAP* pOldTarget = al_get_target_bitmap();
	al_set_target_bitmap(dest.getBitmap());
	draw(destLoc, sprite, scale);
	al_set_target_bitmap(pOldTarget);
}

void GraphicsSystem::writeText(const Vector2D& destLoc, const Font& font, const Color& color, const std::string& text, Font::Alignment align /*= Font::LEFT*/)
{
	al_draw_text(
		font.getFont(),
		getAllegroColorFromColor(color),
		destLoc.getX(),
		destLoc.getY(),
		getAllegroFontAlignFlag(align),
		text.c_str());
}

void GraphicsSystem::writeText(GraphicsBuffer& dest, const Vector2D& destLoc, const Font& font, const Color& color, const std::string& text, Font::Alignment align /*= Font::LEFT*/)
{
	ALLEGRO_BITMAP* pOldTarget = al_get_target_bitmap();
	al_set_target_bitmap(dest.getBitmap());
	writeText(destLoc, font, color, text, align);
	al_set_target_bitmap(pOldTarget);

}

void GraphicsSystem::setBufferToColor(GraphicsBuffer& buffer, const Color& color)
{
	ALLEGRO_BITMAP* pOldTarget = al_get_target_bitmap();
	al_set_target_bitmap(buffer.getBitmap());
	al_clear_to_color(getAllegroColorFromColor(color));
	al_set_target_bitmap(pOldTarget);
}

void GraphicsSystem::saveBufferToFile(const GraphicsBuffer& buffer, const std::string& filename)
{
	bool success = al_save_bitmap(filename.c_str(), buffer.getBitmap());
	assert(success);
}

ALLEGRO_COLOR GraphicsSystem::getAllegroColorFromColor(const Color& color)
{
	return al_map_rgba(color.mR, color.mG, color.mB, color.mA);
}

int GraphicsSystem::getAllegroFontAlignFlag(Font::Alignment align)
{
	int flag;

	switch (align)
	{
	case Font::LEFT:
		flag = ALLEGRO_ALIGN_LEFT;
		break;
	case Font::CENTER:
		flag = ALLEGRO_ALIGN_CENTER;
		break;
	case Font::RIGHT:
		flag = ALLEGRO_ALIGN_RIGHT;
		break;
	default:
		flag = ALLEGRO_ALIGN_LEFT;
	}

	return flag;
}
