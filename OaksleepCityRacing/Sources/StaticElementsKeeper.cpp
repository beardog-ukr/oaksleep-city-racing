#include "StaticElementsKeeper.h"
using namespace oaksleep_city_racing;

#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

USING_NS_CC;
using namespace std;

static const int kMoveActionTag = 22;

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

StaticElementsKeeper::StaticElementsKeeper() {
  camera = nullptr;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

StaticElementsKeeper::~StaticElementsKeeper() {
  C6_F1(c6, "here");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void StaticElementsKeeper::doMove(const std::pair<float, float> moveInfo) {
  camera->stopAllActionsByTag(kMoveActionTag);

  const float moveDistance = moveInfo.first;
  const float timeInteval = moveInfo.second;

  const Vec3 currentCameraPos = camera->getPosition3D();
  Vec3 newCameraPos = Vec3(currentCameraPos.x, currentCameraPos.y + moveDistance,
                           currentCameraPos.z);

  CallFunc* cf = CallFunc::create([this, moveInfo]() {
    doMove(moveInfo);
  });


  MoveTo* cmt = MoveTo::create(timeInteval, newCameraPos);
//  cmt->setTag(kMoveActionTag);
  Sequence* seq =  Sequence::create(cmt, cf, nullptr);
  seq->setTag(kMoveActionTag);
  camera->runAction(seq);
}


// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

// void ActorNode::doChangePositionTo(const int newGameX, const int newGameY) {
//   C6_D1(c6, "here");

//   Vec2 newPosition = calculatePosition(newGameX, newGameY);

//   string an;
//   if (newGameX == gameX) {
//     if (newGameY > gameY) {
//       an = kMoveAnimationNames.north;
//     }
//     else {
//       an = kMoveAnimationNames.south;
//     }
//   }
//   else {
//     if (newGameX > gameX) {
//       an = kMoveAnimationNames.east;
//     }
//     else {
//       an = kMoveAnimationNames.west;
//     }
//   }
//   Animation* animation = AnimationCache::getInstance()->getAnimation(an);
//   animation->setDelayPerUnit(iterationDuration/animation->getTotalDelayUnits());
//   Animate* animate = Animate::create(animation);

//   CallFunc *cf = CallFunc::create([this]() {
//     this->setSpriteFrame(kDefaultFrameName);
//   });

//   Sequence* seq = Sequence::create(animate, cf, nullptr);
//   runAction(seq);

//   MoveTo* ma = MoveTo::create(iterationDuration, newPosition);
//   runAction(ma);

//   gameX = newGameX;
//   gameY = newGameY;

// }

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

// bool PlayerCarNode::initSelf() {
//   if (!initWithSpriteFrameName(kRedCarFrameName)) {
//     C6_C2(c6, "Failed to init with file ", kRedCarFrameName);
//     return false;    //
//   }

//   return true;
// }

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void StaticElementsKeeper::setCamera(Camera *cameraPtr) {
  camera = cameraPtr;
}

// // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

// void PlayerCarNode::setInitialPos(const cocos2d::Vec2 carPos) {
//   expectedCarPos = carPos;

//   setPosition(carPos);
// }

// // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

// void PlayerCarNode::setLines(const float leftLineX, const float rightLineX) {
//   currentLineIndex = 1;
//   linePositions[0] = leftLineX;
//   linePositions[1] = rightLineX;
// }
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
