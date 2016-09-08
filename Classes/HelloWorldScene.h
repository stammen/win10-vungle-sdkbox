#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Vungle-winrt.h"

class HelloWorld : public cocos2d::Layer, public  sdkbox::VungleListener
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

private:
	virtual void onVungleCacheAvailable(bool isAvailable) override;
	virtual void onVungleStarted() override;
	virtual void onVungleFinished() override;
	virtual void onVungleAdViewed(bool isComplete) override;
	virtual void onVungleAdReward(const std::string& name) override;

    cocos2d::EventListenerCustom* mVungleListener;
    cocos2d::Menu* mMainMenu;
};

#endif // __HELLOWORLD_SCENE_H__
