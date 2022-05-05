#pragma once

#include "EventSystem.h"
#include "EventListener.h"

class SoundSystem : public EventListener
{
public:
	SoundSystem();
	~SoundSystem() {};

private:
	virtual void handleEvent(const Event& theEvent);

};


