#include "GameEndWindowNode.h"
using namespace oaksleep_city_racing;

#include "road/RoadScene.h"
#include "ui/MainMenuScene.h"
#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"

#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

USING_NS_CC;
using namespace std;

static const struct {
  string window;
  string buttonClose;
  string buttonPlay;
} kSpriteFileNames = {
  .window = "ocr_game/main/base_window",
  .buttonClose = "ocr_game/main/button_close",
  .buttonPlay = "ocr_game/main/button_replay",
};

static const string fontMainLabel = "fonts/Marker Felt.ttf";

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

GameEndWindowNode::GameEndWindowNode() {
  //
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

GameEndWindowNode::~GameEndWindowNode() {
  C6_F1(c6, "here");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

GameEndWindowNode* GameEndWindowNode::create(const bool victoryAchieved,
                                             shared_ptr<SixCatsLogger> c6) {

  GameEndWindowNode *pRet = new(std::nothrow) GameEndWindowNode();
  if (pRet ==nullptr) {
    return nullptr;
  }

  pRet->victoryMode = victoryAchieved;
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

bool GameEndWindowNode::initMainLabel() {
  const string victoryCaption = "Victory!!!";
  const string failCaption = "You failed...";
  const string caption = victoryMode ? victoryCaption : failCaption;

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

bool GameEndWindowNode::initMenuButtons() {
  SpriteFrameCache* sfc = SpriteFrameCache::getInstance();
  const Size cws = getContentSize();
  SpriteFrame*sf;

  // --- play button
  sf = sfc->getSpriteFrameByName(kSpriteFileNames.buttonPlay);
  if (sf == nullptr) {
    C6_D2(c6, "Failed to find ", kSpriteFileNames.buttonPlay);
    return false;
  }
  MenuItemImage* replayButton = MenuItemImage::create();
  replayButton->setNormalSpriteFrame(sf);
  replayButton->setCallback(CC_CALLBACK_1(GameEndWindowNode::mcReplay, this));
  replayButton->setPosition(cws.width/4, cws.height/8);

  // --- close button
  sf = sfc->getSpriteFrameByName(kSpriteFileNames.buttonClose);
  if (sf == nullptr) {
    C6_D2(c6, "Failed to find ", kSpriteFileNames.buttonClose);
    return false;
  }
  MenuItemImage* closeButton = MenuItemImage::create();
  closeButton->setNormalSpriteFrame(sf);
  closeButton->setCallback(CC_CALLBACK_1(GameEndWindowNode::mcClose, this));
  closeButton->setPosition(cws.width/2 + cws.width/4, cws.height/8);

  // --- assembling menu
  auto menu = Menu::create(replayButton, closeButton, nullptr);
  menu->setPosition(Vec2::ZERO);
  addChild(menu, kMainMenuSceneZO.button);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool GameEndWindowNode::initSelf() {
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

void GameEndWindowNode::mcClose(cocos2d::Ref *pSender) {
  // c6->d(__c6_MN__, "Need to get out.");

  // Close the cocos2d-x game scene and quit the application
  // Director::getInstance()->end();
  Scene* mainMenuScene = MainMenuScene::createScene(c6);
  if (mainMenuScene == nullptr) {
    C6_D1(c6, "Failed to create main menu scene for some reason");
    return;
  }
  //
  Director::getInstance()->replaceScene(mainMenuScene);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void GameEndWindowNode::mcReplay(cocos2d::Ref *pSender) {
  Scene* roadScene = RoadScene::createScene(c6);
  if (roadScene == nullptr) {
    C6_D1(c6, "Failed to create road scene for some reason");
    return;
  }
  //
  Director::getInstance()->replaceScene(roadScene);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

