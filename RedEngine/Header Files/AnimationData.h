#include "Trackable.h"
#include <vector>

class Sprite;
class GraphicsBuffer;

class AnimationData : public Trackable
{

public:
    friend class AnimationManager;

    Sprite* getSprite(int index);
    int getFrameCount() { return mSprites.size(); }

private:
    AnimationData() = delete;
    AnimationData(Sprite* frames, int numOfFrames);
	AnimationData(GraphicsBuffer* gb, int rows, int columns, float scale = 1.0f);
	~AnimationData();

    std::vector<Sprite*> mSprites;
	bool mOwnsSprites;

};