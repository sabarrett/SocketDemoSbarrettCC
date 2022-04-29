#pragma once
#include "GameObject.h"
class PlayerMarcher :
	public GameObject
{
private:
	const float MAX_SPEED = 0.1f;
	const float MAX_FORCE = 0.1f;
	Vector3 mGoal;
	Vector3 mVelocity;
	bool mIsServer;

	PlayerMarcher();
	PlayerMarcher(bool isServer);
	PlayerMarcher(bool isServer, int posX, int posY);
	~PlayerMarcher();
protected:
public:
	virtual void Update(WorldState* gameWorld, int delta);
	void SetGoalPos(int posX, int posY, bool isServer);
	void SetIsServer(bool isServer);
	virtual void Write(OutputMemoryBitStream& stream);
	virtual void Read(InputMemoryBitStream& stream);

	CLASS_IDENTIFICATION('MRCH', PlayerMarcher)
};

