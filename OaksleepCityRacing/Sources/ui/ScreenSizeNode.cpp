#include "ScreenSizeNode.h"
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

static const int kScreenSizeCaptionsCount = 4;
static const string kScreenSizeCaptions[kScreenSizeCaptionsCount] = {
  "360x640", "480x800", "768x1024", "768x1360"
};


// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

ScreenSizeNode::ScreenSizeNode() {
  valueLabel = nullptr;
  selectedSettingIdx = 0;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

ScreenSizeNode::~ScreenSizeNode() {
  C6_F1(c6, "here");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void ScreenSizeNode::changeValue(cocos2d::Ref *pSender, const bool doIncrement) {
  if (valueLabel == nullptr) {
    return;
  }
  if ((selectedSettingIdx == 0) && (doIncrement==false)) {
    return;
  }
  if ((selectedSettingIdx == (kScreenSizeCaptionsCount-1)) && (doIncrement==true)) {
    return;
  }

  if (doIncrement) {
    selectedSettingIdx += 1;
  }
  else {
    selectedSettingIdx -= 1;
  }

  valueLabel->setString(kScreenSizeCaptions[selectedSettingIdx]);
}


// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

ScreenSizeNode* ScreenSizeNode::create(shared_ptr<UiWindowsManager> uwm,
                                       shared_ptr<SixCatsLogger> c6) {

  ScreenSizeNode *pRet = new(std::nothrow) ScreenSizeNode();
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

bool ScreenSizeNode::initApplyLabel() {
  const Size cws = getContentSize();
  Label* label;
  // --- button label
  string caption = "*changes\n will be applied\n after next launch";
  label = Label::createWithTTF(caption, kFontMainLabel, 48);
  if (label == nullptr) {
    C6_D2(c6, "Failed to create label, font ", kFontMainLabel);
    return false;
  }
  label->setTextColor(Color4B(160,82,65,255));
  label->setAnchorPoint(Vec2(0.5,0.5));
  const Size cs = getContentSize();
  label->setPosition(cs.width/2, cs.height*0.4);
  addChild(label);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool ScreenSizeNode::initCaptionLabel() {
  const string caption = "Screen Size";
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

bool ScreenSizeNode::initMenuButtons() {
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
  backButton->setCallback(CC_CALLBACK_1(ScreenSizeNode::mcBack, this));
  backButton->setPosition(cws.width/2 + cws.width/4, cws.height/8);

  // --- assembling menu
  auto menu = Menu::create(backButton, nullptr);
  menu->setPosition(Vec2::ZERO);
  addChild(menu, kMainMenuSceneZO.button);


  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool ScreenSizeNode::initSelf() {
  if (!initWithSpriteFrameName(kSpriteFileNames.window)) {
    C6_C2(c6, "Failed to init with file ", kSpriteFileNames.window);
    return false;    //
  }

  if (!initCaptionLabel()) {
    return false;
  }

  if (!initApplyLabel()) {
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

bool ScreenSizeNode::initValuePanel() {
  const Size cs = getContentSize();
  const float rowPos = cs.height*0.7;
  SpriteFrameCache* sfc = SpriteFrameCache::getInstance();
  SpriteFrame*sf;

  // --- label
  valueLabel = Label::createWithTTF(kScreenSizeCaptions[selectedSettingIdx], kFontMainLabel, 64);
  if (valueLabel == nullptr) {
    C6_D2(c6, "Failed to create label, font ", kFontMainLabel);
    return false;
  }
  valueLabel->setTextColor(Color4B(250,250,250,255));
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
  lessButton->setCallback(CC_CALLBACK_1(ScreenSizeNode::changeValue, this, false));
  lessButton->setPosition(cs.width*0.2, rowPos);

  sf = sfc->getSpriteFrameByName(kSpriteFileNames.buttonMore);
  if (sf == nullptr) {
    C6_D2(c6, "Failed to find ", kSpriteFileNames.buttonMore);
    return false;
  }
  MenuItemImage* moreButton = MenuItemImage::create();
  moreButton->setNormalSpriteFrame(sf);
  moreButton->setCallback(CC_CALLBACK_1(ScreenSizeNode::changeValue, this, true));
  moreButton->setPosition(cs.width*0.8, rowPos);

  // --- assembling menu
  auto menu = Menu::create(lessButton, moreButton, nullptr);
  menu->setPosition(Vec2::ZERO);
  addChild(menu, kMainMenuSceneZO.button);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void ScreenSizeNode::mcBack(cocos2d::Ref *pSender) {
  C6_T1(c6, "Need to get back from screen size settings.");
  uiWindowsManager->swapBack(this);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

//void ScreenSizeNode::mcApply(cocos2d::Ref *pSender) {
//  C6_D1(c6, "button callback here");
//  auto director = Director::getInstance();
//  auto glview = director->getOpenGLView();
//  glview->setFrameSize(768, 1024);
//  director->setOpenGLView(glview);
//}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

