#pragma once

#include "Trackable.h"
#include "Vector2D.h"

class RCamera2D : public Trackable
{

public:
    RCamera2D();
    RCamera2D(Vector2D location);
    ~RCamera2D();

    void setLoc(Vector2D location) { mLoc = location; }

    Vector2D getLoc() { return mLoc; }

private:
    Vector2D mLoc;

};