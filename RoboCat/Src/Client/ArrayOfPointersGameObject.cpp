#include "Client/ArrayOfPointersGameObject.h"
#include "Util/RoboCatPCH.h"

ArrayOfPointersGameObject::ArrayOfPointersGameObject()
{
}

ArrayOfPointersGameObject::~ArrayOfPointersGameObject()
{
}

void ArrayOfPointersGameObject::Init(int UID, uint8_t textureID)
{
	GameObject::Init(UID, textureID);
	objectType = 1;
}

void ArrayOfPointersGameObject::Update(std::vector<GameObject*> collidableObjects)
{
	GameObject::Update(collidableObjects);
	CheckCollisions(collidableObjects);
	for (size_t i = 0; i < m_TargetObjects.size(); i++)
	{
		m_TargetObjects[i]->SetTextureID((uint8_t)((m_TargetObjects[i]->GetTextureID() + 1 % 3)+1));
	}
}

void ArrayOfPointersGameObject::CleanUp()
{
}

bool ArrayOfPointersGameObject::CheckCollisions(std::vector<GameObject*> collidableObjects)
{
	for (size_t i = 0; i < collidableObjects.size(); i++)
	{
		if (abs(collidableObjects[i]->GetPosX() - m_xPos) < 1 && abs(collidableObjects[i]->GetPosY() - m_yPos) < 1)
		{
			if (collidableObjects[i] != this)
			{
				m_TargetObjects.push_back(collidableObjects[i]);
			}
		}
	}
	if (collidableObjects.size() > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ArrayOfPointersGameObject::AddTargetObject(GameObject* targetObject)
{
	if (targetObject)
	{
		m_TargetObjects.push_back(targetObject);
	}
	containsUpdatedInfo = true;
}

void ArrayOfPointersGameObject::Write(OutputMemoryBitStream* outStream)
{
	outStream->Write(objectType);
	outStream->Write(containsUpdatedInfo);
	if (containsUpdatedInfo)
	{
		outStream->Write(m_textureID);
		outStream->Write(m_xPos);
		outStream->Write(m_yPos);

		std::cout << "Sent: " << m_TargetObjects.size() << "\n";
		outStream->Write(m_TargetObjects.size());
		for (size_t i = 0; i < m_TargetObjects.size(); i++)
		{
			std::cout << m_TargetObjects[i]->GetUID() << "\n";
			outStream->Write(m_TargetObjects[i]->GetUID());
		}

		containsUpdatedInfo = false;
	}
}

void ArrayOfPointersGameObject::Read(InputMemoryBitStream* inStream)
{
	inStream->Read(m_textureID);
	inStream->Read(m_xPos);
	inStream->Read(m_yPos);
}
