#include "Font.h"
#include "Font.h"
#include <cassert>

using namespace std;

Font::Font(const std::string& filename, int size)
	:mSize(size)
{
	mpFont = al_load_font(filename.c_str(), size, 0);
	assert(mpFont);
}

Font::~Font()
{
	al_destroy_font(mpFont);
}
