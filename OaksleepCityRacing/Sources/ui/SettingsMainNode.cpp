#include "SettingsMainNode.h"
using namespace oaksleep_city_racing;

#include "ui/GameplaySettingsNode.h"
#include "ui/ScreenSizeNode.h"
#include "ui/SoundSettingsNode.h"
#include "ui/UiWindowsManager.h"
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
  string commonButton;
  string commonButtonPressed;
} kSpriteFileNames = {
  .window = "ocr_game/main/base_window",
  .buttonExit = "ocr_game/main/button_close",
  .buttonPlay = "ocr_game/main/button_play",
  .buttonSettings = "ocr_game/main/button_settings",
  .commonButton = "ocr_game/main/button_base",
  .commonButtonPressed = "ocr_game/main/button_base2"
};

static const string kFontMainLabel = "fonts/Marker Felt.ttf";

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

SettingsMainNode::SettingsMainNode() {
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

SettingsMainNode::~SettingsMainNode() {
  C6_F1(c6, "here");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

SettingsMainNode* SettingsMainNode::create(shared_ptr<UiWindowsManager> uwm,
                                           shared_ptr<SixCatsLogger> c6) {

  SettingsMainNode *pRet = new(std::nothrow) SettingsMainNode();
  if (pRet ==nullptr) {
    return nullptr;
  }

  pRet->uiWindowsManager = uwm;
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

bool SettingsMainNode::initCaptionLabel() {
  const string caption = "Settings";
  Label* label = Label::createWithTTF(caption, kFontMainLabel, 56);
  if (label == nullptr) {
    C6_D2(c6, "Failed to create label, font ", kFontMainLabel);
    return false;
  }
  label->setTextColor(Color4B(160,82,65,255));
  label->setAnchorPoint(Vec2(0.5,0.5));
  const Size cws = getContentSize();
  label->setPosition(cws.width/2, cws.height*0.9);
  addChild(label);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool SettingsMainNode::initMainButtons() {
  SpriteFrameCache* sfc = SpriteFrameCache::getInstance();
  const Size cws = getContentSize();

  Menu* menu = Menu::create();

  const int buttonCount = 3;
  const string captions[buttonCount] = { "Sound", "Difficulty", "Screen Size"};
  const ccMenuCallback callbacks[buttonCount] = {
    CC_CALLBACK_1(SettingsMainNode::mcSound, this),
    CC_CALLBACK_1(SettingsMainNode::mcDifficulty, this),
    CC_CALLBACK_1(SettingsMainNode::mcScreenSize, this),
  };
  const float posMods[buttonCount] = {0.7, 0.5, 0.3};

  for(int i = 0; i<buttonCount; i++) {
    SpriteFrame*sf;
    Label* label;

    // --- button image
    MenuItemImage* button = MenuItemImage::create();
    sf = sfc->getSpriteFrameByName(kSpriteFileNames.commonButton);
    if (sf == nullptr) {
      C6_D2(c6, "Failed to find ", kSpriteFileNames.commonButton);
      return false;
    }
    button->setNormalSpriteFrame(sf);

    sf = sfc->getSpriteFrameByName(kSpriteFileNames.commonButtonPressed);
    if (sf == nullptr) {
      C6_D2(c6, "Failed to find ", kSpriteFileNames.commonButtonPressed);
      return false;
    }
    button->setSelectedSpriteFrame(sf);

    button->setCallback(callbacks[i]);
    button->setPosition(cws.width/2, cws.height*posMods[i]);

    // --- button label
    label = Label::createWithTTF(captions[i], kFontMainLabel, 64);
    if (label == nullptr) {
      C6_D2(c6, "Failed to create label, font ", kFontMainLabel);
      return false;
    }
    label->setTextColor(Color4B(160,82,65,255));
    label->setAnchorPoint(Vec2(0.5,0.5));
    const Size bs = button->getContentSize();
    label->setPosition(bs.width/2, bs.height/2);
    button->addChild(label);

    // ---
    menu->addChild(button);
  }

  // --- final touches on menu
  menu->setPosition(Vec2::ZERO);
  addChild(menu, kMainMenuSceneZO.button);


  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool SettingsMainNode::initMenuButtons() {
  SpriteFrameCache* sfc = SpriteFrameCache::getInstance();
  const Size cws = getContentSize();
  SpriteFrame*sf;

  // --- exit button
  sf = sfc->getSpriteFrameByName(kSpriteFileNames.buttonExit);
  if (sf == nullptr) {
    C6_D2(c6, "Failed to find ", kSpriteFileNames.buttonExit);
    return false;
  }
  MenuItemImage* backButton = MenuItemImage::create();
  backButton->setNormalSpriteFrame(sf);
  backButton->setCallback(CC_CALLBACK_1(SettingsMainNode::mcBack, this));
  backButton->setPosition(cws.width/2 + cws.width/4, cws.height/8);

  // --- assembling menu
  auto menu = Menu::create(backButton, nullptr);
  menu->setPosition(Vec2::ZERO);
  addChild(menu, kMainMenuSceneZO.button);


  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool SettingsMainNode::initSelf() {
  if (!initWithSpriteFrameName(kSpriteFileNames.window)) {
    C6_C2(c6, "Failed to init with file ", kSpriteFileNames.window);
    return false;    //
  }

  if (!initCaptionLabel()) {
    return false;
  }

  if (!initMainButtons()) {
    return false;
  }

  if (!initMenuButtons()) {
    return false;
  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void SettingsMainNode::mcBack(cocos2d::Ref *pSender) {
  c6->d(__c6_MN__, "Need to get back from main settings.");
  uiWindowsManager->swapBack(this);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void SettingsMainNode::mcDifficulty(cocos2d::Ref *pSender) {
  C6_D1(c6, "Difficulty button callback here");

  GameplaySettingsNode* gsNode = GameplaySettingsNode::create(uiWindowsManager,c6);
  if (gsNode == nullptr) {
    return;
  }

  gsNode->setScale(getScale());
  getParent()->addChild(gsNode, kMainMenuSceneZO.window);

  uiWindowsManager->swapNodes(this, gsNode);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void SettingsMainNode::mcScreenSize(cocos2d::Ref *pSender) {
  C6_D1(c6, "Screen size button callback here");
  ScreenSizeNode* sssNode = ScreenSizeNode::create(uiWindowsManager,c6);
  if (sssNode == nullptr) {
    return;
  }

  sssNode->setScale(getScale());
  getParent()->addChild(sssNode, kMainMenuSceneZO.window);

  uiWindowsManager->swapNodes(this, sssNode);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void SettingsMainNode::mcSound(cocos2d::Ref *pSender) {
  C6_D1(c6, "Sound button callback here");

  SoundSettingsNode* smNode = SoundSettingsNode::create(uiWindowsManager,c6);
  if (smNode == nullptr) {
    return;
  }

  smNode->setScale(getScale());
  getParent()->addChild(smNode, kMainMenuSceneZO.window);

  uiWindowsManager->swapNodes(this, smNode);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

