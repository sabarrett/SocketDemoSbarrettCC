#pragma once
#include<iostream>
#include"../common/DeanLib/include/Trackable.h"
#include<vector>

/*
Author: Nicholas Preis
	Class : Game Architecture <250-71>
	Assignment : Assignment 4
	Certification of Authenticity :
I certify that this assignment is entirely my own work.
*/

using namespace std;

class Unit;
class Animation;
class System;

class UnitManager : public Trackable
{
public:
	UnitManager();
	~UnitManager();

	void addUnits(Unit* units);
	Unit* getUnits(int numUnits);
	void deleteUnits(int deletedUnits);

	void clearUnits();
	void updateUnits(float elapsedTime);

	int getPosX() { return mPosX; }
	int getPosY() { return mPosY; }

	void setPosX(int posX) { mPosX = posX; };
	void setPosY(int posY) { mPosY = posY; };

	void draw(System* system);

	int getNumUnits();
	void cleanup();
private:
	vector<Unit*>* mpUnits = nullptr;

	int mPosX;
	int mPosY;
};

