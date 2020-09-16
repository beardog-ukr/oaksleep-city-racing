#include "StaticElementsKeeper.h"
using namespace oaksleep_city_racing;
#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"

#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

USING_NS_CC;
using namespace std;

static const int kMoveActionTag = 22;
static const string kLifesSpriteFrameName = "ocr_game/ui_road/hp_full";

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

StaticElementsKeeper::StaticElementsKeeper() {
  camera = nullptr;

  lifesCounter = 5;
  lifesLabel = nullptr;
  lifesSprite = nullptr;
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

bool StaticElementsKeeper::initLifesWidget(Scene* roadScene) {
  lifesSprite = Sprite::createWithSpriteFrameName(kLifesSpriteFrameName);
  if (lifesSprite == nullptr) {
    C6_W2(c6, "Failed to open ", kLifesSpriteFrameName);
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

  camera = roadScene->getDefaultCamera();
  if (camera == nullptr) {
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

void StaticElementsKeeper::doMoveLifeIndicator(const float moveDistance, const float timeInterval) {
  lifesSprite->stopAllActionsByTag(kMoveActionTag);

  const Vec2 currentPos = lifesSprite->getPosition();
  Vec2 newPos = Vec2(currentPos.x, currentPos.y + moveDistance);

  MoveTo* mt = MoveTo::create(timeInterval, newPos);
  mt->setTag(kMoveActionTag);
  lifesSprite->runAction(mt);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
