#pragma once
#include "GameObject.h"

class Lock : public GameObject
{
	private:
		Lock();
		Lock(int posX, int posY);
		~Lock();
	protected:
	public:
		virtual void Write(OutputMemoryBitStream& stream);
		virtual void Read(InputMemoryBitStream& stream);
		virtual void Update();

		CLASS_IDENTIFICATION('LOCK', Lock)
};