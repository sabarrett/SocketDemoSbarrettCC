#include "Client/ColorChangerGameObject.h"
#include "Util/RoboCatPCH.h"

void ColorChangerGameObject::Init(int UID, uint8_t textureID)
{
	GameObject::Init(UID, textureID);
	m_objectType = GAMEOBJECT_COLORCHANGER;
}

void ColorChangerGameObject::Update(std::vector<GameObject*> collidableObjects)
{
	GameObject::Update(collidableObjects);
	CheckCollisions(collidableObjects);
	
	if (m_colorChangeFramesPast >= m_colorChangeFrameDelay)
	{
		for (size_t i = 0; i < m_TargetObjects.size(); i++)
		{
			int texID = m_TargetObjects[i]->GetTextureID() + 1;
			if (texID > 4)
			{
				texID = 2;
			}
			m_TargetObjects[i]->SetTextureID((uint8_t)(texID));
		}
		m_colorChangeFramesPast = -1;
	}
	m_colorChangeFramesPast++;
}

void ColorChangerGameObject::CleanUp()
{
	for (size_t i = 0; i < m_TargetObjects.size(); i++)
	{
		m_TargetObjects.erase(m_TargetObjects.begin() + i);
	}
}

void ColorChangerGameObject::CheckCollisions(std::vector<GameObject*> collidableObjects)
{
	for (size_t i = 0; i < collidableObjects.size(); i++)
	{
		if (collidableObjects[i] != this && collidableObjects[i]->GetType() != GAMEOBJECT_COLLECTABLE)
		{
			float dist = abs(collidableObjects[i]->GetPosX() - m_xPos) + abs(collidableObjects[i]->GetPosY() - m_yPos);
			if (dist <= 120)
			{
				bool alreadyTargeted = false;
				for (size_t j = 0; j < m_TargetObjects.size(); j++)
				{
					if (collidableObjects[i] == m_TargetObjects[j])
					{
						alreadyTargeted = true;
					}
				}
				if (!alreadyTargeted)
				{
					m_TargetObjects.push_back(collidableObjects[i]);
				}
			}
		}
	}
}

void ColorChangerGameObject::AddTargetObject(GameObject* targetObject)
{
	if (targetObject)
	{
		m_TargetObjects.push_back(targetObject);
	}
	m_containsUpdatedInfo = true;
}

void ColorChangerGameObject::Draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
	SDL_Rect dstRect;
	dstRect.x = (int)m_xPos-64;
	dstRect.y = (int)m_yPos-64;
	dstRect.h = 128;
	dstRect.w = 128;
	SDL_RenderCopy(renderer, texture, NULL, &dstRect);
}

void ColorChangerGameObject::Write(OutputMemoryBitStream* outStream)
{
	if (m_containsUpdatedInfo)
	{
		outStream->Write(m_textureID);
		outStream->Write(m_xPos);
		outStream->Write(m_yPos);

		outStream->Write(m_TargetObjects.size());
		for (size_t i = 0; i < m_TargetObjects.size(); i++)
		{
			outStream->Write(m_TargetObjects[i]->GetUID());
		}
		m_containsUpdatedInfo = false;
	}
}

void ColorChangerGameObject::Read(InputMemoryBitStream* inStream)
{
	inStream->Read(m_textureID);
	inStream->Read(m_xPos);
	inStream->Read(m_yPos);
}
