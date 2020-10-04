#include "AppDelegate.h"
#include "road/RoadScene.h"
#include "ui/GameEndScene.h"
#include "ui/MainMenuScene.h"

// #define USE_AUDIO_ENGINE 1

#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#endif

USING_NS_CC;

//static cocos2d::Size designResolutionSize = cocos2d::Size(768, 1024);
static cocos2d::Size designResolutionSize = cocos2d::Size(360, 640);

AppDelegate::AppDelegate() {
  //Logging level for the application is set here in next line
  c6 = std::make_shared<SixCatsLogger>(SixCatsLogger::Debug);
}

AppDelegate::~AppDelegate() {
#if USE_AUDIO_ENGINE
  AudioEngine::end();
#endif
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs() {
  // set OpenGL context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
  GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};

  GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,
// don't modify or remove this function
static int register_all_packages() {
  return 0;   //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
  // initialize director
  auto director = Director::getInstance();
  auto glview = director->getOpenGLView();
  if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || \
    (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    glview =
      GLViewImpl::createWithRect("OaksleepCityRacing",
                                 cocos2d::Rect(0, 0, designResolutionSize.width,
                                               designResolutionSize.height));
#else
    glview = GLViewImpl::create("OaksleepCityRacing");
#endif
    director->setOpenGLView(glview);
  }

  // turn on display FPS
  director->setDisplayStats(true);

  // set FPS. the default value is 1.0/60 if you don't call this
  director->setAnimationInterval(1.0f / 60);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
  // Here goes a hack to get real screen resolution of the device and then fit to it

  // First, set some super-huge design resolution, no matter what
  static cocos2d::Size designResolutionSize = cocos2d::Size(3600, 6400);
  glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height,
                                  ResolutionPolicy::EXACT_FIT);

  // Second, get real resolution ans set it
  Size frameSize = glview->getFrameSize();
  glview->setDesignResolutionSize(frameSize.width, frameSize.height, ResolutionPolicy::EXACT_FIT);

  // Note, all the scenes are able to scale their content appropriately to their size
#else
    // Setting design resolution for desktop applications, nothing special
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height,
                                  ResolutionPolicy::EXACT_FIT);
#endif

  register_all_packages();

  // create a scene. it's an autorelease object
//  Scene* scene = oaksleep_city_racing::RoadScene::createScene(c6);
  Scene* scene = oaksleep_city_racing::MainMenuScene::createScene(c6);
//  Scene* scene = oaksleep_city_racing::GameEndScene::createScene(false, c6);
  if (scene == nullptr) {
    C6_C1(c6, "Failed to create a scene.");
    return false;
  }

  // run
  director->runWithScene(scene);

  return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
  Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
  AudioEngine::pauseAll();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
  Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
  AudioEngine::resumeAll();
#endif
}
