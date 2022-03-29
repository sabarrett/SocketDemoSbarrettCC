
#include "RoboCatPCH.h"
#include "Key.h"


Key::Key() : GameObject("Key_blue.png", 70, 70)
{

}

Key::Key(int posX, int posY) : GameObject("Key_blue.png", posX, posY, 70, 70)
{

}

Key::~Key()
{
}

void Key::Update()
{
}
