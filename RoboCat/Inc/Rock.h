#pragma once

/*
	File information:
	This file contains the definition for the Rock class, derived from GameObject
*/

#include "GameObject.h"
//#include "TransmissionData.h"

class Rock : public GameObject/*, public TransmissionData*/
{
	//-------------------------Private data-------------------------


	//-------------------------Public data-------------------------
public:

	//Constructor(s)
	Rock(const int networkID, GraphicsLibrary* graphicsLibrary);
	Rock(const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position, const std::string spriteIdentifier);

	//Overloaded functions
	void HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager, const PacketSequenceNumber packetSequenceNum) const;
	void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager, const PacketSequenceNumber packetSequenceNum) const;

	//Destructor
	~Rock();

	//Accessor(s)


	//Mutator(s)


	//Functions
	void update();
	void draw();
};