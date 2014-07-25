#include "AppDelegate.h"
#include "StartScene.h"


USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLView::create("SushiCrush");
		glview->setFrameSize(320.0,480.0);
        director->setOpenGLView(glview);
    }

	glview->setDesignResolutionSize(320.0, 480.0, ResolutionPolicy::FIXED_WIDTH);
	std::vector<std::string> searchPath;
	searchPath.push_back("w640");
	CCFileUtils::getInstance()->setSearchPaths(searchPath);
	director->setContentScaleFactor(640.0 / 320.0);

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    auto scene = Start::create();

    // run
    director->runWithScene(scene);



    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
