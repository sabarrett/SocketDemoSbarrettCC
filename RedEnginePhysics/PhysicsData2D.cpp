#include "PhysicsData2D.h"

PhysicsData2D::PhysicsData2D()
{
    mPos = Vector2D::Zero();
    mVel = Vector2D::Zero();
    mAcc = Vector2D::Zero();

    mMass = 1.0f;
    mFacing = 0.0f;
    mRotVel = 0.0f;
    mRotAcc = 0.0f;
    mDampingConstant = 1.0f;
}

PhysicsData2D::PhysicsData2D(Vector2D pos, Vector2D vel, Vector2D acc, 
    float mass, float facing, float rotVel, float rotAcc, float dampingConstant)
{
    mPos = pos;
    mVel = vel;
    mAcc = acc;

    mMass = mass;
    mFacing = facing;
    mRotVel = rotVel;
    mRotAcc = rotAcc;
    mDampingConstant = dampingConstant;
}