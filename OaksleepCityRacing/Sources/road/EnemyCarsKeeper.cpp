#include "EnemyCarsKeeper.h"
using namespace oaksleep_city_racing;

#include "EnemyCarNode.h"
#include "PlayerCarNode.h"

#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"

#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

USING_NS_CC;
using namespace std;

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

static int getRandomECKValue() {
  int result = 0;
  result = RandomHelper::random_int((int)0, 99);

  return result;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

EnemyCarsKeeper::EnemyCarsKeeper() {
  roadLength = 0;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

EnemyCarsKeeper::~EnemyCarsKeeper() {
  C6_F1(c6, "here");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool EnemyCarsKeeper::generateCars(const int inRoadLength, cocos2d::Node* carsParent) {
  roadLength = inRoadLength;

  const Size currentWindowSize = carsParent->getContentSize();

  const float leftLane = currentWindowSize.width/2 - currentWindowSize.width/8;
  const float rightLane = currentWindowSize.width/2 + currentWindowSize.width/8;

  int lengthAcc = 0;//PlayerCarNode::kTurnDistance*3;
  bool needEmptyZone = true;
  int previousLaneId;

  while (lengthAcc < roadLength) {
    if (needEmptyZone) {
      lengthAcc += PlayerCarNode::kTurnDistance*3;
    }

    int rv = getRandomECKValue();

    if (rv<40) {
      // add empty section on the road
      lengthAcc += PlayerCarNode::kTurnDistance*3;
      needEmptyZone = false;
      previousLaneId = -1; // -1 is value for "empty zone"
    }
    else {
      EnemyCarNode* enemyCar = EnemyCarNode::create(c6);
      if (enemyCar == nullptr) {
        return false;
      }

      enemyCar->setLanes(leftLane, rightLane);

      int initialLane = 0;
      if (rv>80) {
        initialLane = 1;
      }

      if ( ((previousLaneId == 0) && (initialLane == 1)) ||
           ((previousLaneId == 1) && (initialLane == 0)) ) {
        // add one empty section
        lengthAcc += PlayerCarNode::kTurnDistance*3;
      }

      enemyCar->setInitialPos(initialLane, lengthAcc);

      enemyCar->setRoadLength(roadLength);
      carsParent->addChild(enemyCar, kRoadSceneZO.enemyCar);

      enemyCar->doMove();
      cars.push_back(enemyCar);

      lengthAcc += PlayerCarNode::kTurnDistance*3;
    }
  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

EnemyCarsKeeper* EnemyCarsKeeper::create(shared_ptr<SixCatsLogger> c6) {

  EnemyCarsKeeper *pRet = new(std::nothrow) EnemyCarsKeeper();
  if (pRet ==nullptr) {
    return nullptr;
  }

  pRet->setLogger(c6);

  if (!pRet->initSelf()) {
    delete pRet;
    pRet = nullptr;
    return nullptr;
  }

//  pRet->autorelease();
  return pRet;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool EnemyCarsKeeper::initSelf() {
  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

