#pragma once

#include <Trackable.h>
#include <Vector2D.h>
#include <vector>

class Unit;
class Animation;
class Sprite;

class UnitManager : public Trackable
{
public:
	UnitManager();
	~UnitManager();

	void clear();//remove and delete all units

	void update(double dt);
	void draw() const;

	Unit* createUnit(const Vector2D& position, const Animation& mainAnimation, const Animation& altAnimation);
	Unit* createUnit(const Vector2D& position, Sprite& sprite);
	Unit* createUnit(const Animation& mainAnimation, const Animation& altAnimation, int seed);

	void deleteAllUnitsAt2DPosition(const Vector2D& position);
	bool deleteUnitAt2DPosition(const Vector2D& position);//return true if unit found and deleted - false otherwise
	void deleteUnitAtIndex(unsigned int index);
	void deleteUnit(Unit* pUnit);

	Unit* getLastUnitCreated() const;
	void setPauseStateForAllAnimations( bool isPaused );
	void togglePauseStateForAllAnimations();

	unsigned int getNumUnits() const { return mUnits.size(); };

private:
	std::vector<Unit*> mUnits;
	uint32_t nextID;
};
