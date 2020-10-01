#include "StaticElementsKeeper.h"
using namespace oaksleep_city_racing;
#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"

#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

USING_NS_CC;
using namespace std;

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

static const int kMoveActionTag = 22;

static const int kLifeWidgetMax = 5;
static const string kLifeWidgetSprites[kLifeWidgetMax] = {
  "ocr/ui_road/hp_0", "ocr/ui_road/hp_1", "ocr/ui_road/hp_2",
  "ocr/ui_road/hp_3", "ocr/ui_road/hp_4"
};

static const int kGearIndicatorMax = 4;
static const string kGearIndicatorSprites[kGearIndicatorMax] = {
  "ocr/ui_road/gear_1", "ocr/ui_road/gear_2",
  "ocr/ui_road/gear_3", "ocr/ui_road/gear_4",
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

StaticElementsKeeper::StaticElementsKeeper() {
  camera = nullptr;

  lifesSprite = nullptr;
  gearSprite = nullptr;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

StaticElementsKeeper::~StaticElementsKeeper() {
  C6_F1(c6, "here");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

StaticElementsKeeper* StaticElementsKeeper::create(Scene* roadScene, shared_ptr<SixCatsLogger> c6) {

  StaticElementsKeeper *pRet = new(std::nothrow) StaticElementsKeeper();
  if (pRet ==nullptr) {
    return nullptr;
  }

  pRet->setLogger(c6);

  if (!pRet->initSelf(roadScene)) {
    delete pRet;
    pRet = nullptr;
    return nullptr;
  }

//  pRet->autorelease();
  return pRet;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool StaticElementsKeeper::initGearIndicator(Scene* roadScene) {
  gearSprite = Sprite::createWithSpriteFrameName(kGearIndicatorSprites[0]);
  if (gearSprite == nullptr) {
    C6_W2(c6, "Failed to open: ", kGearIndicatorSprites[0]);
    return false;
  }

  const Size rscs = roadScene->getContentSize();
  gearSprite->setAnchorPoint(Vec2(1,1));
  gearSprite->setPosition(rscs.width,rscs.height);

  roadScene->addChild(gearSprite, kRoadSceneZO.gearIndicator);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool StaticElementsKeeper::initLifesWidget(Scene* roadScene) {
//  const string ss = "ocr_game/ui_road/hp_full";
  lifesSprite = Sprite::createWithSpriteFrameName(kLifeWidgetSprites[0]);
  if (lifesSprite == nullptr) {
    C6_W2(c6, "Failed to open: ", kLifeWidgetSprites[0]);
    return false;
  }

  const Size rscs = roadScene->getContentSize();
  lifesSprite->setAnchorPoint(Vec2(0,1));
  lifesSprite->setPosition(0,rscs.height);

  roadScene->addChild(lifesSprite, kRoadSceneZO.lifesIndicator);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool StaticElementsKeeper::initSelf(cocos2d::Scene* roadScene) {
  if (roadScene == nullptr) {
    return false;
  }

  windowSize = roadScene->getContentSize();

  camera = roadScene->getDefaultCamera();
  if (camera == nullptr) {
    return false;
  }

  if (!initGearIndicator(roadScene)) {
    return false;
  }

  if (!initLifesWidget(roadScene)) {
    return false;
  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void StaticElementsKeeper::doMove(const std::pair<float, float> moveInfo) {
  const float moveDistance = moveInfo.first;
  const float timeInterval = moveInfo.second;

  doMoveCamera(moveDistance, timeInterval);
  doMoveGearIndicator(moveDistance, timeInterval);
  doMoveLifeIndicator(moveDistance, timeInterval);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void StaticElementsKeeper::doMoveCamera(const float moveDistance, const float timeInterval) {
  camera->stopAllActionsByTag(kMoveActionTag);

  const Vec3 currentCameraPos = camera->getPosition3D();
  Vec3 newCameraPos = Vec3(currentCameraPos.x, currentCameraPos.y + moveDistance,
                           currentCameraPos.z);

  MoveTo* cmt = MoveTo::create(timeInterval, newCameraPos);
  cmt->setTag(kMoveActionTag);
  camera->runAction(cmt);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void StaticElementsKeeper::doMoveGearIndicator(const float moveDistance, const float timeInterval) {
  gearSprite->stopAllActionsByTag(kMoveActionTag);

  const Vec2 currentPos = gearSprite->getPosition();
  Vec2 newPos = Vec2(currentPos.x, currentPos.y + moveDistance);

  MoveTo* mt = MoveTo::create(timeInterval, newPos);
  mt->setTag(kMoveActionTag);
  gearSprite->runAction(mt);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void StaticElementsKeeper::doMoveLifeIndicator(const float moveDistance, const float timeInterval) {
  lifesSprite->stopAllActionsByTag(kMoveActionTag);

  const Vec2 currentPos = lifesSprite->getPosition();
  Vec2 newPos = Vec2(currentPos.x, currentPos.y + moveDistance);

  MoveTo* mt = MoveTo::create(timeInterval, newPos);
  mt->setTag(kMoveActionTag);
  lifesSprite->runAction(mt);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void StaticElementsKeeper::setGearIndicator(const int value) {
  const int idx = value -1;
  if ((idx<0)||(idx >= kGearIndicatorMax)) {
    return;
  }

  if (gearSprite == nullptr) {
    return;    // impossible
  }

  gearSprite->setSpriteFrame(kGearIndicatorSprites[idx]);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void StaticElementsKeeper::setLifesCounter(const int value) {
  if ((value<0)||(value >= kLifeWidgetMax)) {
    return;
  }

  if (lifesSprite == nullptr) {
    return;    // impossible
  }

  lifesSprite->setSpriteFrame(kLifeWidgetSprites[value]);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void StaticElementsKeeper::setYPosition(const int yPos) {
  gearSprite->setPosition(windowSize.width, yPos + windowSize.height/2);
  lifesSprite->setPosition(0,yPos + windowSize.height/2);

  const Vec3 currentCameraPos = camera->getPosition3D();
  Vec3 newCameraPos = Vec3(currentCameraPos.x, yPos, currentCameraPos.z);
  camera->setPosition3D(newCameraPos);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
