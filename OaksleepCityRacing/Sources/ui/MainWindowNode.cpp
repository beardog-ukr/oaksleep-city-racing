#include "MainWindowNode.h"
using namespace oaksleep_city_racing;

#include "road/RoadScene.h"
#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"

#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

USING_NS_CC;
using namespace std;



static const struct {
  string window;
  string buttonExit;
  string buttonPlay;
  string buttonSettings;
} kSpriteFileNames = {
  .window = "ocr_game/main/base_window",
  .buttonExit = "ocr_game/main/button_close",
  .buttonPlay = "ocr_game/main/button_play",
  .buttonSettings = "ocr_game/main/button_settings",
};

static const string fontMainLabel = "fonts/Marker Felt.ttf";

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

MainWindowNode::MainWindowNode() {
  //
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

MainWindowNode::~MainWindowNode() {
  C6_F1(c6, "here");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

MainWindowNode* MainWindowNode::create(shared_ptr<SixCatsLogger> c6) {

  MainWindowNode *pRet = new(std::nothrow) MainWindowNode();
  if (pRet ==nullptr) {
    return nullptr;
  }

  pRet->setLogger(c6);

  if (!pRet->initSelf()) {
    delete pRet;
    pRet = nullptr;
    return nullptr;
  }

  pRet->autorelease();
  return pRet;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool MainWindowNode::initMainLabel() {
  const string caption = "Oaksleep city\n      racing";
  Label* label = Label::createWithTTF(caption, fontMainLabel, 64);
  if (label == nullptr) {
    C6_D2(c6, "Failed to create label, font ", fontMainLabel);
    return false;
  }
  label->setTextColor(Color4B(160,82,65,255));
  label->setAnchorPoint(Vec2(0.5,0.5));
  const Size cws = getContentSize();
  label->setPosition(cws.width/2, cws.height/2);
  addChild(label);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool MainWindowNode::initMenuButtons() {
  SpriteFrameCache* sfc = SpriteFrameCache::getInstance();
  const Size cws = getContentSize();
  SpriteFrame*sf;

  // --- settings button
  sf = sfc->getSpriteFrameByName(kSpriteFileNames.buttonSettings);
  if (sf == nullptr) {
    C6_D2(c6, "Failed to find ", kSpriteFileNames.buttonSettings);
    return false;
  }
  MenuItemImage* settingsButton = MenuItemImage::create();
  settingsButton->setNormalSpriteFrame(sf);
  settingsButton->setCallback(CC_CALLBACK_1(MainWindowNode::mcSettings, this));
  settingsButton->setPosition(cws.width/2, cws.height/8);

  // --- play button
  sf = sfc->getSpriteFrameByName(kSpriteFileNames.buttonPlay);
  if (sf == nullptr) {
    C6_D2(c6, "Failed to find ", kSpriteFileNames.buttonPlay);
    return false;
  }
  MenuItemImage* playButton = MenuItemImage::create();
  playButton->setNormalSpriteFrame(sf);
  playButton->setCallback(CC_CALLBACK_1(MainWindowNode::mcNewGame, this));
  playButton->setPosition(cws.width/4, cws.height/8);

  // --- exit button
  sf = sfc->getSpriteFrameByName(kSpriteFileNames.buttonExit);
  if (sf == nullptr) {
    C6_D2(c6, "Failed to find ", kSpriteFileNames.buttonExit);
    return false;
  }
  MenuItemImage* exitButton = MenuItemImage::create();
  exitButton->setNormalSpriteFrame(sf);
  exitButton->setCallback(CC_CALLBACK_1(MainWindowNode::mcExit, this));
  exitButton->setPosition(cws.width/2 + cws.width/4, cws.height/8);

  // --- assembling menu
  auto menu = Menu::create(playButton, settingsButton, exitButton, nullptr);
  menu->setPosition(Vec2::ZERO);
  addChild(menu, kMainMenuSceneZO.button);


  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool MainWindowNode::initSelf() {
  if (!initWithSpriteFrameName(kSpriteFileNames.window)) {
    C6_C2(c6, "Failed to init with file ", kSpriteFileNames.window);
    return false;    //
  }

  if (!initMainLabel()) {
    return false;
  }

  if (!initMenuButtons()) {
    return false;
  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void MainWindowNode::mcExit(cocos2d::Ref *pSender) {
  c6->d(__c6_MN__, "Need to get out.");

  // Close the cocos2d-x game scene and quit the application
  Director::getInstance()->end();
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void MainWindowNode::mcNewGame(cocos2d::Ref *pSender) {
  Scene* roadScene = RoadScene::createScene(c6);
  if (roadScene == nullptr) {
    C6_D1(c6, "Failed to create scene for some reason");
    return;
  }
  //
  Director::getInstance()->replaceScene(roadScene);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void MainWindowNode::mcSettings(cocos2d::Ref *pSender) {
  C6_D1(c6, "Not implemented yet");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

