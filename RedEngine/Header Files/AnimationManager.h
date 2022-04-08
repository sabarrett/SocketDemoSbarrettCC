#include "Trackable.h"
#include <vector>
#include <unordered_map>
#include <string>

class Animation;
class AnimationData;
class Sprite;
class GraphicsBuffer;

class AnimationManager : public Trackable
{

public:
    AnimationManager();
    ~AnimationManager();

    int getNumOfAnimations() { return mAnimations.size(); }
    int getNumOfAnimData() { return mAnimData.size(); }

    Animation* createAnimation(AnimationData* data, int fps);
    Animation* createAndManageAnimation(AnimationData* data, int fps);

    AnimationData* createAnimationData(Sprite* frames, int numOfFrames);
    AnimationData* createAnimationData(GraphicsBuffer* gb, int rows, int columns, float scale = 1.0f);
    AnimationData* createAndManageAnimationData(std::string key, Sprite* frames, int numOfFrames);
    AnimationData* createAndManageAnimationData(std::string key, GraphicsBuffer* gb, int rows, int columns, float scale = 1.0f);

    void addAnimation(Animation* anim);
    void removeAnimation(Animation* anim);
    void removeAndDeleteAnimation(Animation* anim);
    void deleteAnimation(Animation* anim);

    void addAnimationData(std::string key, AnimationData* data);
    void removeAnimationData(std::string key); 
    void removeAndDeleteAnimationData(std::string key);
    void deleteAnimationData(AnimationData* data);

    int findAnimation(Animation* anim);
    Animation* getAnimationAt(int index);

    std::string findAnimationDataKey(AnimationData* data);
    AnimationData* getAnimationData(std::string key);

    void update(float deltaTime);

    void clear();

private:
    std::vector<Animation*> mAnimations;
    std::unordered_map<std::string, AnimationData*> mAnimData;
};