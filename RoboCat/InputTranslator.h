#pragma once
#include<iostream>
#include"Event.h"
#include"EventListener.h"
#include"EventSystem.h"
#include"InputSystem.h"
#include"GameEvent.h"

/*
Author: Nicholas Preis
	Class : Game Architecture <250-71>
	Assignment : Assignment 5
	Certification of Authenticity :
I certify that this assignment is entirely my own work.
*/

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

