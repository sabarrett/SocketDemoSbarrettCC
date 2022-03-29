#include "Client/CollectableGameObject.h"
#include "Util/RoboCatPCH.h"

void CollectableGameObject::Init(int UID, uint8_t textureID)
{
	GameObject::Init(UID, textureID);
	m_objectType = GAMEOBJECT_COLLECTABLE;
}

void CollectableGameObject::Update(std::vector<GameObject*> collidableObjects)
{
	GameObject::Update(collidableObjects);

	for (size_t i = 0; i < collidableObjects.size(); i++)
	{
		if (collidableObjects[i] != this && collidableObjects[i]->GetType() != GetType())
		{
			float dist = abs(collidableObjects[i]->GetPosX() - m_xPos) + abs(collidableObjects[i]->GetPosY() - m_yPos);
			if (dist <= 30)
			{
				SetShouldDestroy(true);
			}
		}
	}
}

void CollectableGameObject::CleanUp()
{
}

void CollectableGameObject::Write(OutputMemoryBitStream* outStream)
{
	if (m_containsUpdatedInfo)
	{
		outStream->Write(m_textureID);
		outStream->Write(m_xPos);
		outStream->Write(m_yPos);
		m_containsUpdatedInfo = false;
	}
}

void CollectableGameObject::Read(InputMemoryBitStream* inStream)
{
	bool readMore;
	inStream->Read(readMore);
	if (readMore)
	{
		inStream->Read(m_textureID);
		inStream->Read(m_xPos);
		inStream->Read(m_yPos);
	}
}
