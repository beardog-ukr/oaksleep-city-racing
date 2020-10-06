#include "SoundSettingsNode.h"
using namespace oaksleep_city_racing;

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
  string buttonLess;
  string buttonMore;
} kSpriteFileNames = {
  .window = "ocr_game/main/base_window",
  .buttonExit = "ocr_game/main/button_close",
  .buttonPlay = "ocr_game/main/button_play",
  .buttonSettings = "ocr_game/main/button_settings",
  .commonButton = "ocr_game/main/button_base",
  .commonButtonPressed = "ocr_game/main/button_base2",
  .buttonLess = "ocr_game/buttons/left",
  .buttonMore = "ocr_game/buttons/right"
};

static const string kFontMainLabel = "fonts/Marker Felt.ttf";

static const int kSvIncStep = 5;
static const int kSvMax = 100;
static const int kSvMin = 0;

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

static string createValueLabelText(const int value) {
  ostringstream oss;
  oss << value;
  oss << "%";
  return oss.str();
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

SoundSettingsNode::SoundSettingsNode() {
  valueLabel = nullptr;
  soundValue = 50;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

SoundSettingsNode::~SoundSettingsNode() {
  C6_F1(c6, "here");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void SoundSettingsNode::changeValue(cocos2d::Ref *pSender, const bool doIncrement) {
  if (valueLabel == nullptr) {
    return;
  }
  if ((soundValue == kSvMin) && (doIncrement==false)) {
    return;
  }
  if ((soundValue == kSvMax) && (doIncrement==true)) {
    return;
  }

  if (doIncrement) {
    soundValue += kSvIncStep;
    if (soundValue > kSvMax) {
      soundValue = kSvMax;
    }
  }
  else {
    soundValue -= kSvIncStep;
    if (soundValue < kSvMin) {
      soundValue = kSvMin;
    }
  }

  valueLabel->setString(createValueLabelText(soundValue));
}


// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

SoundSettingsNode* SoundSettingsNode::create(shared_ptr<UiWindowsManager> uwm,
                                             shared_ptr<SixCatsLogger> c6) {

  SoundSettingsNode *pRet = new(std::nothrow) SoundSettingsNode();
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

bool SoundSettingsNode::initCaptionLabel() {
  const string caption = "Sound Settings";
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

bool SoundSettingsNode::initMainButton() {
  SpriteFrameCache* sfc = SpriteFrameCache::getInstance();
  const Size cws = getContentSize();

  Menu* menu = Menu::create();

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

  button->setCallback(CC_CALLBACK_1(SoundSettingsNode::mcSwitch, this));
  button->setPosition(cws.width/2, cws.height*0.7);

  // --- button label
  label = Label::createWithTTF("Turn Off", kFontMainLabel, 64);
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


  // --- final touches on menu
  menu->setPosition(Vec2::ZERO);
  addChild(menu, kMainMenuSceneZO.button);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool SoundSettingsNode::initMenuButtons() {
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
  backButton->setCallback(CC_CALLBACK_1(SoundSettingsNode::mcBack, this));
  backButton->setPosition(cws.width/2 + cws.width/4, cws.height/8);

  // --- assembling menu
  auto menu = Menu::create(backButton, nullptr);
  menu->setPosition(Vec2::ZERO);
  addChild(menu, kMainMenuSceneZO.button);


  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool SoundSettingsNode::initSelf() {
  if (!initWithSpriteFrameName(kSpriteFileNames.window)) {
    C6_C2(c6, "Failed to init with file ", kSpriteFileNames.window);
    return false;    //
  }

  if (!initCaptionLabel()) {
    return false;
  }

  if (!initMainButton()) {
    return false;
  }

  if (!initMenuButtons()) {
    return false;
  }

  if (!initValuePanel()) {
    return false;
  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool SoundSettingsNode::initValuePanel() {
  const Size cs = getContentSize();
  const float rowPos = cs.height*0.4;
  SpriteFrameCache* sfc = SpriteFrameCache::getInstance();
  SpriteFrame*sf;

  // --- label
  valueLabel = Label::createWithTTF("__", kFontMainLabel, 72);
  if (valueLabel == nullptr) {
    C6_D2(c6, "Failed to create label, font ", kFontMainLabel);
    return false;
  }
  valueLabel->setTextColor(Color4B(250,250,250,255));
  valueLabel->setString(createValueLabelText(soundValue));
  valueLabel->setAnchorPoint(Vec2(0.5,0.5));
  valueLabel->setPosition(cs.width/2, rowPos);
  addChild(valueLabel);

  // --- "<" and ">" buttons
  sf = sfc->getSpriteFrameByName(kSpriteFileNames.buttonLess);
  if (sf == nullptr) {
    C6_D2(c6, "Failed to find ", kSpriteFileNames.buttonLess);
    return false;
  }
  MenuItemImage* lessButton = MenuItemImage::create();
  lessButton->setNormalSpriteFrame(sf);
  lessButton->setCallback(CC_CALLBACK_1(SoundSettingsNode::changeValue, this, false));
  lessButton->setPosition(cs.width*0.2, rowPos);

  sf = sfc->getSpriteFrameByName(kSpriteFileNames.buttonMore);
  if (sf == nullptr) {
    C6_D2(c6, "Failed to find ", kSpriteFileNames.buttonMore);
    return false;
  }
  MenuItemImage* moreButton = MenuItemImage::create();
  moreButton->setNormalSpriteFrame(sf);
  moreButton->setCallback(CC_CALLBACK_1(SoundSettingsNode::changeValue, this, true));
  moreButton->setPosition(cs.width*0.8, rowPos);

  // --- assembling menu
  auto menu = Menu::create(lessButton, moreButton, nullptr);
  menu->setPosition(Vec2::ZERO);
  addChild(menu, kMainMenuSceneZO.button);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void SoundSettingsNode::mcBack(cocos2d::Ref *pSender) {
  C6_T1(c6, "Need to get back from sound settings.");
  uiWindowsManager->swapBack(this);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void SoundSettingsNode::mcSwitch(cocos2d::Ref *pSender) {
  C6_D1(c6, "Difficulty button callback here (not implemented)");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

