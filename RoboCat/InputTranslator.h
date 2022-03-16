#pragma once
#include<iostream>
#include"GraphicsLib/Event.h"
#include"GraphicsLib/EventListener.h"
#include"GraphicsLib/EventSystem.h"
#include"GraphicsLib/InputSystem.h"
#include"GameEvent.h"

using namespace std;

class InputTranslator : public EventListener
{
public:
	InputTranslator(InputSystem* input);
	~InputTranslator();

	void init();
	void cleanup();

	void handleEvent(const Event& event);
private:
	EventSystem* mEventSystem;
	InputSystem* mInputSystem;

	bool mDeleted;
};
