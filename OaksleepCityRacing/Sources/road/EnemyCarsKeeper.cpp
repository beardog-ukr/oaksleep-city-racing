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

bool EnemyCarsKeeper::generateCars(const RoadInfo &roadInfo, cocos2d::Node* carsParent) {
  const Size currentWindowSize = carsParent->getContentSize();

  int lengthAcc = roadInfo.startPosition;
  bool needEmptyZone = false;
  int previousLaneId = 0;

  const int kProbEmptyZone = 30;// probability of an empty zone to appear
  const int kProbRightLane = 75;// probability for the cat to appear in right(slow) lane

  const float emptyZoneLength = roadInfo.turnDistance*2;

  while (lengthAcc < roadInfo.roadLength) {
    if (needEmptyZone) {
      lengthAcc += emptyZoneLength;
    }

    int rv = getRandomECKValue();

    if (rv<kProbEmptyZone) {
      // add empty section on the road
      lengthAcc += emptyZoneLength; //
      needEmptyZone = false;
      previousLaneId = -1; // -1 is value for "empty zone"
    }
    else {
      EnemyCarNode* enemyCar = EnemyCarNode::create(c6);
      if (enemyCar == nullptr) {
        return false;
      }

      enemyCar->setRoadInfo(roadInfo);

      int initialLane = 0;
      if (rv>kProbRightLane) {
        initialLane = 1;
      }

      if ( ((previousLaneId == 0) && (initialLane == 1)) ||
           ((previousLaneId == 1) && (initialLane == 0)) ) {
        // add one empty section
        lengthAcc += emptyZoneLength;
      }

      enemyCar->setInitialPos(initialLane, lengthAcc);

      carsParent->addChild(enemyCar, kRoadSceneZO.enemyCar);

      cars.push_back(enemyCar);

      lengthAcc += emptyZoneLength;
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

//  pRet->autorelease(); see task #1d45fcf79a
  return pRet;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool EnemyCarsKeeper::initSelf() {
  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void EnemyCarsKeeper::startCars() {
  for(EnemyCarNode* ecn: cars) {
    ecn->doMove();
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

