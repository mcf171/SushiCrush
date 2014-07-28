#include "StartLayer.h"
#include "PlayLayer.h"

StartLayer::StartLayer()
{

};

StartLayer::~StartLayer()
{

};

bool StartLayer::init()
{
	Size winSize = Director::getInstance()->getWinSize();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// init background
	auto background = Sprite::create("menubackground2.jpg");
	background->setAnchorPoint(Vec2(0, 1));
	background->setPosition(Vec2(0, winSize.height));
	this->addChild(background, -1);

	// init lable
	auto label = LabelTTF::create("Sushi Crush", "Arial", 36);
	label->setColor(ccc3(255,255,0));
	label->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - label->getContentSize().height));
	this->addChild(label, 1);

	// init menu
	auto newgameItem = MenuItemImage::create(
											 "newgame.png",
											 "newgame.png",
											 CC_CALLBACK_1(StartLayer::newgameCallback,this));
	auto continueItem = MenuItemImage::create(
											 "continue.png",
										 	 "continue.png",
											 CC_CALLBACK_1(StartLayer::continueCallback, this));
	auto settingsItem = MenuItemImage::create(
											 "settings.png",
											 "settings.png",
											 CC_CALLBACK_1(StartLayer::settingsCallback, this));
	auto exitItem = MenuItemImage::create(
											 "exit.png",
											 "exit.png",
											 CC_CALLBACK_1(StartLayer::exitCallback, this));
	newgameItem->setPosition(Vec2(origin.x + visibleSize.width / 2, 
		origin.y + visibleSize.height - 117 - 88 + newgameItem->getContentSize().height));
	continueItem->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - 117 - 88*2 + continueItem->getContentSize().height));
	settingsItem->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - 117 - 88*3 + settingsItem->getContentSize().height));
	exitItem->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - 117 - 88*4 + exitItem->getContentSize().height));
	auto menu = Menu::create(newgameItem,continueItem,settingsItem,exitItem,NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu,1);

	return true;
};

void StartLayer::newgameCallback(Ref* pSender)
{
	Director::sharedDirector()->popToRootScene();
	auto scene = PlayLayer::createScene();
	Director::sharedDirector()->replaceScene(scene);
};

void StartLayer::continueCallback(Ref* pSender)
{
	Director::sharedDirector()->popScene();
};

void StartLayer::settingsCallback(Ref* pSender)
{

};

void StartLayer::exitCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.", "Alert");
	return;
#endif

	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
};
