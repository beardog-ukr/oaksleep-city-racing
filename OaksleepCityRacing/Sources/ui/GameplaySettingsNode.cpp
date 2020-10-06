#include "GameplaySettingsNode.h"
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

static const int kProbIncStep = 5;
static const int kCarProbMax = 85;
static const int kCarProbMin = 10;
static const int kRightLaneProbMax = 85;
static const int kRightLaneProbMin = 10;

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

static string createValueLabelText(const int value) {
  ostringstream oss;
  oss << value;
  oss << "%";
  return oss.str();
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

GameplaySettingsNode::GameplaySettingsNode() {
  carProbLabel = nullptr;
  rightLaneProbLabel = nullptr;
  carProb = 50;
  rightLaneProb = 40;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

GameplaySettingsNode::~GameplaySettingsNode() {
  C6_F1(c6, "here");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void GameplaySettingsNode::changeCarProbValue(cocos2d::Ref *pSender, const bool doIncrement) {
  if (carProbLabel == nullptr) {
    return;
  }
  if ((carProb == kCarProbMin) && (doIncrement==false)) {
    return;
  }
  if ((carProb == kCarProbMax) && (doIncrement==true)) {
    return;
  }

  if (doIncrement) {
    carProb += kProbIncStep;
    if (carProb > kCarProbMax) {
      carProb = kCarProbMax;
    }
  }
  else {
    carProb -= kProbIncStep;
    if (carProb < kCarProbMin) {
      carProb = kCarProbMin;
    }
  }

  carProbLabel->setString(createValueLabelText(carProb));
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void GameplaySettingsNode::changeRightLaneProbValue(cocos2d::Ref *pSender,
                                                    const bool doIncrement) {
  if (rightLaneProbLabel == nullptr) {
    return;
  }
  if ((rightLaneProb == kRightLaneProbMin) && (doIncrement==false)) {
    return;
  }
  if ((rightLaneProb == kRightLaneProbMax) && (doIncrement==true)) {
    return;
  }

  if (doIncrement) {
    rightLaneProb += kProbIncStep;
    if (rightLaneProb > kRightLaneProbMax) {
      rightLaneProb = kRightLaneProbMax;
    }
  }
  else {
    rightLaneProb -= kProbIncStep;
    if (rightLaneProb < kRightLaneProbMin) {
      rightLaneProb = kRightLaneProbMin;
    }
  }

  rightLaneProbLabel->setString(createValueLabelText(rightLaneProb));
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

GameplaySettingsNode* GameplaySettingsNode::create(shared_ptr<UiWindowsManager> uwm,
                                                   shared_ptr<SixCatsLogger> c6) {

  GameplaySettingsNode *pRet = new(std::nothrow) GameplaySettingsNode();
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

bool GameplaySettingsNode::initCaptionLabel() {
  const string caption = "Gameplay Settings";
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

bool GameplaySettingsNode::initCarProbPanel() {
  const Size cs = getContentSize();
  const float rowPos = cs.height*0.65;
  SpriteFrameCache* sfc = SpriteFrameCache::getInstance();
  SpriteFrame*sf;

  // --- label
  Label* clabel = Label::createWithTTF("Car probability", kFontMainLabel, 36);
  if (clabel == nullptr) {
    C6_D2(c6, "Failed to create label, font ", kFontMainLabel);
    return false;
  }
  clabel->setTextColor(Color4B(250,250,250,255));
  clabel->setAnchorPoint(Vec2(0.5,0.5));
  clabel->setPosition(cs.width*0.35, cs.height*0.8);
  addChild(clabel);

  // --- value label
  carProbLabel = Label::createWithTTF("__", kFontMainLabel, 72);
  if (carProbLabel == nullptr) {
    C6_D2(c6, "Failed to create label, font ", kFontMainLabel);
    return false;
  }
  carProbLabel->setTextColor(Color4B(250,250,250,255));
  carProbLabel->setString(createValueLabelText(carProb));
  carProbLabel->setAnchorPoint(Vec2(0.5,0.5));
  carProbLabel->setPosition(cs.width/2, rowPos);
  addChild(carProbLabel);

  // --- "<" and ">" buttons
  sf = sfc->getSpriteFrameByName(kSpriteFileNames.buttonLess);
  if (sf == nullptr) {
    C6_D2(c6, "Failed to find ", kSpriteFileNames.buttonLess);
    return false;
  }
  MenuItemImage* lessButton = MenuItemImage::create();
  lessButton->setNormalSpriteFrame(sf);
  lessButton->setCallback(CC_CALLBACK_1(GameplaySettingsNode::changeCarProbValue, this, false));
  lessButton->setPosition(cs.width*0.2, rowPos);

  sf = sfc->getSpriteFrameByName(kSpriteFileNames.buttonMore);
  if (sf == nullptr) {
    C6_D2(c6, "Failed to find ", kSpriteFileNames.buttonMore);
    return false;
  }
  MenuItemImage* moreButton = MenuItemImage::create();
  moreButton->setNormalSpriteFrame(sf);
  moreButton->setCallback(CC_CALLBACK_1(GameplaySettingsNode::changeCarProbValue, this, true));
  moreButton->setPosition(cs.width*0.8, rowPos);

  // --- assembling menu
  auto menu = Menu::create(lessButton, moreButton, nullptr);
  menu->setPosition(Vec2::ZERO);
  addChild(menu, kMainMenuSceneZO.button);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool GameplaySettingsNode::initMenuButtons() {
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
  backButton->setCallback(CC_CALLBACK_1(GameplaySettingsNode::mcBack, this));
  backButton->setPosition(cws.width/2 + cws.width/4, cws.height/8);

  // --- assembling menu
  auto menu = Menu::create(backButton, nullptr);
  menu->setPosition(Vec2::ZERO);
  addChild(menu, kMainMenuSceneZO.button);


  return true;
}
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool GameplaySettingsNode::initRightLaneProbPanel() {
  const Size cs = getContentSize();
  const float rowPos = cs.height*0.3;
  SpriteFrameCache* sfc = SpriteFrameCache::getInstance();
  SpriteFrame*sf;

  // --- label
  Label* clabel = Label::createWithTTF("Car in right lane probability", kFontMainLabel, 36);
  if (clabel == nullptr) {
    C6_D2(c6, "Failed to create label, font ", kFontMainLabel);
    return false;
  }
  clabel->setTextColor(Color4B(250,250,250,255));
  clabel->setAnchorPoint(Vec2(0.5,0.5));
  clabel->setPosition(cs.width*0.5, cs.height*0.45);
  addChild(clabel);

  // --- value label
  rightLaneProbLabel = Label::createWithTTF("__", kFontMainLabel, 72);
  if (rightLaneProbLabel == nullptr) {
    C6_D2(c6, "Failed to create label, font ", kFontMainLabel);
    return false;
  }
  rightLaneProbLabel->setTextColor(Color4B(250,250,250,255));
  rightLaneProbLabel->setString(createValueLabelText(rightLaneProb));
  rightLaneProbLabel->setAnchorPoint(Vec2(0.5,0.5));
  rightLaneProbLabel->setPosition(cs.width/2, rowPos);
  addChild(rightLaneProbLabel);

  // --- "<" and ">" buttons
  sf = sfc->getSpriteFrameByName(kSpriteFileNames.buttonLess);
  if (sf == nullptr) {
    C6_D2(c6, "Failed to find ", kSpriteFileNames.buttonLess);
    return false;
  }
  MenuItemImage* lessButton = MenuItemImage::create();
  lessButton->setNormalSpriteFrame(sf);
  lessButton->setCallback(
    CC_CALLBACK_1(GameplaySettingsNode::changeRightLaneProbValue, this, false));
  lessButton->setPosition(cs.width*0.2, rowPos);

  sf = sfc->getSpriteFrameByName(kSpriteFileNames.buttonMore);
  if (sf == nullptr) {
    C6_D2(c6, "Failed to find ", kSpriteFileNames.buttonMore);
    return false;
  }
  MenuItemImage* moreButton = MenuItemImage::create();
  moreButton->setNormalSpriteFrame(sf);
  moreButton->setCallback(
    CC_CALLBACK_1(GameplaySettingsNode::changeRightLaneProbValue, this, true));
  moreButton->setPosition(cs.width*0.8, rowPos);

  // --- assembling menu
  auto menu = Menu::create(lessButton, moreButton, nullptr);
  menu->setPosition(Vec2::ZERO);
  addChild(menu, kMainMenuSceneZO.button);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool GameplaySettingsNode::initSelf() {
  if (!initWithSpriteFrameName(kSpriteFileNames.window)) {
    C6_C2(c6, "Failed to init with file ", kSpriteFileNames.window);
    return false;    //
  }

  if (!initCaptionLabel()) {
    return false;
  }

  if (!initRightLaneProbPanel()) {
    return false;
  }

  if (!initMenuButtons()) {
    return false;
  }

  if (!initCarProbPanel()) {
    return false;
  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void GameplaySettingsNode::mcBack(cocos2d::Ref *pSender) {
  C6_T1(c6, "Need to get back from gameplay settings.");
  uiWindowsManager->swapBack(this);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

