#pragma once

/*
	File information:
	This file contains the definition for the PlayerController class, derived from GameObject
*/

#include "GameObject.h"
#include "InputSystem.h"

class PlayerController : public GameObject, public TransmissionData
{
	//-------------------------Private data-------------------------

	//Reference to input system
	InputSystem* pInputKeyDown;
	InputSystem* pInputKeyUp;

	bool bShouldMoveUp = false;
	bool bShouldMoveDown = false;
	bool bShouldMoveLeft = false;
	bool bShouldMoveRight = false;

	//Movement data
	float mMoveSpeed;

	//-------------------------Public data-------------------------
public:

	//Constructor(s)
	PlayerController(const int networkID, GraphicsLibrary* graphicsLibrary);
	PlayerController(const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position, float moveSpeed, const std::string spriteIdentifier);

	//Overloaded functions
	void HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager, const PacketSequenceNumber packetSequenceNum) const;
	void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager, const PacketSequenceNumber packetSequenceNum) const;

	//Destructor
	~PlayerController();

	//Accessor(s)
	float getMoveSpeed() { return mMoveSpeed; };

	//Mutator(s)
	void setMoveSpeed(float moveSpeed) { mMoveSpeed = moveSpeed; };

	//Functions
	void update();
	void draw();
};