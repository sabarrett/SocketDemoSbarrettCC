#include "Camera2D.h"

RCamera2D::RCamera2D()
{
    mLoc = Vector2D::Zero();
}

RCamera2D::RCamera2D(Vector2D location)
{
    mLoc = location;
}

RCamera2D::~RCamera2D()
{
    
}