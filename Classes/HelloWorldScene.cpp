#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"


USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // Add main menu
    mMainMenu = Menu::create();
    int index = 2;

    std::vector<std::string> vChapters = { "Play Vungle Ad", "Play Incentivized Vungle Ad" };
    std::vector<MenuItemLabel*> vMenuItems;

    // create the menu items for each chaper
    for (size_t i = 0; i < vChapters.size(); i++)
    {
        auto itemLabel = Label::createWithTTF(vChapters.at(i), "fonts/arial.ttf", 24);
        auto menuItemLabel = MenuItemLabel::create(itemLabel);

        menuItemLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2).x,
            (Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height).y - (++index) * 40));

        vMenuItems.push_back(menuItemLabel);
    }

    // create the callback for the Vungle Ad menu item
    vMenuItems.at(0)->setCallback([&](cocos2d::Ref *sender) {
		sdkbox::PluginVungle::show("video");
    });

	// create the callback for the Incentivized Vungle Ad menu item
	vMenuItems.at(1)->setCallback([&](cocos2d::Ref *sender) {
		sdkbox::PluginVungle::show("reward");
	});

    // now add all the menu items to the menu
    for (size_t i = 0; i < vMenuItems.size(); i++)
    {
        mMainMenu->addChild(vMenuItems.at(i), 2);
    }

	// disable the Vungle menu until we receive the onVungleCacheAvailable callback
    mMainMenu->setEnabled(false);
    mMainMenu->setColor(Color3B::RED);
	mMainMenu->setPosition(Vec2::ZERO);

    // add main menu
    this->addChild(mMainMenu, 1);

	// Add the Vungle plugin
	// winrt versions of the Vungle plugin need a reference to the app's Windows::UI::Core::CoreDispatcher^ 
	// in order to start videos on the UI thread. We can get the dispatcher from the winrt CCGLView
	sdkbox::PluginVungle::init((void*)cocos2d::GLViewImpl::sharedOpenGLView()->getDispatcher());

	sdkbox::PluginVungle::setListener(this);

#ifdef COCOS2D_DEBUG
	sdkbox::PluginVungle::setDebug(true);
#endif
    return true;
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
    
    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/
    
    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
    
}

// VungleListener events
void HelloWorld::onVungleCacheAvailable(bool isAvailable)
{
	CCLOG("onVungleCacheAvailable: %s", isAvailable ? "true" : "false");

	// Only access cocos2d-x from cocos2d-x thread
	cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
		if (isAvailable)
		{
			mMainMenu->setColor(Color3B::GREEN);
			mMainMenu->setEnabled(true);
		}
		else
		{
			mMainMenu->setColor(Color3B::RED);
			mMainMenu->setEnabled(false);
		}
	});
}

void HelloWorld::onVungleStarted()
{
	CCLOG("onVungleStarted");
}

void HelloWorld::onVungleFinished()
{
	CCLOG("onVungleFinished");
}

void HelloWorld::onVungleAdViewed(bool isComplete)
{
	CCLOG("onVungleAdViewed: %s", isComplete ? "true" : "false");
}

void HelloWorld::onVungleAdReward(const std::string& name)
{
	CCLOG("onVungleAdReward");
}

