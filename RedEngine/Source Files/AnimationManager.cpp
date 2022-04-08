#include "AnimationManager.h"
#include "AnimationData.h"
#include "Animation.h"

using namespace std;

AnimationManager::AnimationManager()
{

}

AnimationManager::~AnimationManager()
{
    if(!mAnimations.empty())
    {
        clear();
    }
}

Animation* AnimationManager::createAnimation(AnimationData* data, int fps)
{
    return new Animation(data, fps);
}

Animation* AnimationManager::createAndManageAnimation(AnimationData* data, int fps)
{
    Animation* anim = new Animation(data, fps);
    mAnimations.push_back(anim);
    return anim;
}

void AnimationManager::addAnimation(Animation* anim)
{
    mAnimations.push_back(anim);
}

void AnimationManager::removeAnimation(Animation* anim)
{
    for(vector<Animation*>::iterator i = mAnimations.begin(); i != mAnimations.end(); i++)
    {
        if(*i == anim)
        {
            mAnimations.erase(i);
            return;
        }
    }
}

void AnimationManager::removeAndDeleteAnimation(Animation* anim)
{
    for(vector<Animation*>::iterator i = mAnimations.begin(); i != mAnimations.end(); i++)
    {
        if(*i == anim)
        {
            delete *i;
            mAnimations.erase(i);
            return;
        }
    }
}

void AnimationManager::deleteAnimation(Animation* anim)
{
    delete anim;
}

int AnimationManager::findAnimation(Animation* anim)
{
    int index = 0;
    for(vector<Animation*>::iterator i = mAnimations.begin(); i != mAnimations.end(); i++)
    {
        if(*i == anim)
        {
            return index;
        }
        index++;
    }
    return -1;
}

Animation* AnimationManager::getAnimationAt(int index)
{
    int ind = 0;
    for(vector<Animation*>::iterator i = mAnimations.begin(); i != mAnimations.end(); i++)
    {
        if(ind != index)
        {
            ind++;
            continue;
        }

        return *i;
    }
    return nullptr;
}

void AnimationManager::update(float deltaTime)
{
    for(vector<Animation*>::iterator i = mAnimations.begin(); i != mAnimations.end(); i++)
    {
        (*i)->update(deltaTime);
    }
}

void AnimationManager::clear()
{
    if(mAnimations.size() > 0)
    {
        for(vector<Animation*>::iterator i = mAnimations.begin(); i != mAnimations.end(); i++)
        {
            delete (*i);
        }
    }

    if(mAnimData.size() > 0)
    {
        for(unordered_map<string, AnimationData*>::iterator i = mAnimData.begin(); i != mAnimData.end(); i++)
        {
            delete (*i).second;
        }
    }

    mAnimations.clear();
    mAnimData.clear();
}

AnimationData* AnimationManager::createAnimationData(Sprite* frames, int numOfFrames)
{
    return new AnimationData(frames, numOfFrames);
}

AnimationData* AnimationManager::createAnimationData(GraphicsBuffer* gb, int rows, int columns, float scale)
{
    return new AnimationData(gb, rows, columns, scale);
}

AnimationData* AnimationManager::createAndManageAnimationData(string key, Sprite* frames, int numOfFrames)
{
    AnimationData* data = new AnimationData(frames, numOfFrames);
    mAnimData.emplace(key, data);
    return data;
}

AnimationData* AnimationManager::createAndManageAnimationData(string key, GraphicsBuffer* gb, int rows, int columns, float scale)
{
    AnimationData* data = new AnimationData(gb, rows, columns, scale);
    mAnimData.emplace(key, data);
    return data;
}

void AnimationManager::addAnimationData(string key, AnimationData* data)
{
    mAnimData.emplace(key, data);
}

void AnimationManager::removeAnimationData(string key)
{
    mAnimData.erase(key);
}

void AnimationManager::removeAndDeleteAnimationData(string key)
{
    delete mAnimData.at(key);
    mAnimData.erase(key);
}

void AnimationManager::deleteAnimationData(AnimationData* data)
{
    delete data;
}

string AnimationManager::findAnimationDataKey(AnimationData* data)
{
    for(unordered_map<string, AnimationData*>::iterator i = mAnimData.begin(); i != mAnimData.end(); i++)
    {
        if(data == (*i).second)
            return (*i).first;
    }
    return "";
}

AnimationData* AnimationManager::getAnimationData(string key)
{
    return mAnimData.at(key);
}