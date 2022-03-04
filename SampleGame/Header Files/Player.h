#include "Unit.h"
#include "Vector2D.h"

class Player : public Unit
{
public:
    Player();
    Player(Animation* anim, float moveSpeed, Vector2D loc = Vector2D::Zero());
    ~Player();

    void update(double deltaTime);
    void setMoveDirection(Vector2D dir);

};