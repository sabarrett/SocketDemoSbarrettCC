#pragma once

/*
	File information:
	This file contains the definition for the Wall class, derived from GameObject
*/

#include "GameObject.h"
#include "Colour.h"

class Wall : public GameObject, public TransmissionData
{
	//-------------------------Private data-------------------------
	float mSizeX;
	float mSizeY;
	float mThickness;

	Colour mColour;

	//-------------------------Public data-------------------------
public:

	//Constructor(s)
	Wall(const int networkID, GraphicsLibrary* graphicsLibrary);
	Wall(const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position, float sizeX, float sizeY, Colour colour, float thickness);

	//Overloaded functions
	void HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager,  const PacketSequenceNumber packetSequenceNum) const;
	void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager,  const PacketSequenceNumber packetSequenceNum) const;

	//Destructor
	~Wall();

	//Accessor(s)
	float getWallSizeX() { return mSizeX; };
	float getWallSizeY() { return mSizeY; };
	float getWallThickness() { return mThickness; };

	//Mutator(s)
	void setWallSizeX(float sizeX) { mSizeX = sizeX; };
	void setWallSizeY(float sizeY) { mSizeY = sizeY; };
	void setWallThickness(float thiccc) { mThickness = thiccc; };

	//Functions
	void update();
	void draw();
};