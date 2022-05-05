#pragma once

class Animation
{
private:
	float mTimingInfo;
	AnimationPrototype* mPrototype;
};

class AnimationManager
{
public:
	Animation getAnimation(string key);

	void loadAnimation(GraphicsBuffer, sprite sheet info, string key);

	init, cleanup

private:
	map<string, AnimationPrototype*> mMap;
};
