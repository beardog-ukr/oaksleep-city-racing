#include "EnemyCarNode.h"
using namespace oaksleep_city_racing;

#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

USING_NS_CC;
using namespace std;

static const int kGearMax = 4;
static const int kLineMax = 2;

static const int kMoveActionTag = 222;

static const int kSingleMoveDistance = 400;
static const float kSingleMoveInterval = 2.0;
static const int kTurnDistance = 200;

static const string kYellowCarFrameName = "ocr_game/cars/yellow_car";

static const int kYellowCarBodyPointsCount = 8;
static const Vec2 yellowCarBodyPoints[kYellowCarBodyPointsCount] = {
  {.x =       15, .y =          -74.5   },
  {.x =       -12, .y =          -74.5   },
  {.x =       33, .y =          -58.5   },
  {.x =       -33, .y =          -58.5   },
  {.x =       33, .y =          60.5    },
  {.x =       -33, .y =          60.5    },
  {.x =       22, .y =          73.5    },
  {.x =       -22, .y =          73.5    },
};

static const int kPlayerCarCategoryBitmask = 0x01;
static const int kEnemyCarCategoryBitmask = 0x02;

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

EnemyCarNode::EnemyCarNode() {
  currentGear = 1;
  currentLaneIndex = -1;// -1 indicates value was not initialized
  initialY = -1;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

EnemyCarNode::~EnemyCarNode() {
  C6_F1(c6, "here");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

EnemyCarNode* EnemyCarNode::create(shared_ptr<SixCatsLogger> c6) {

  EnemyCarNode *pRet = new(std::nothrow) EnemyCarNode();
  if (pRet ==nullptr) {
    return nullptr;
  }

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

std::pair<float, float> EnemyCarNode::doMove() {
  stopAllActionsByTag(kMoveActionTag); // just in case

  Vec2 currentPos = getPosition();
  float path = roadLength - currentPos.y;
  float velocity = (kSingleMoveDistance * currentGear )/ kSingleMoveInterval;
  float time = path / velocity;
  C6_D4(c6, "Here path = ", path, " time = ", time);

  Vec2 newPos;
  newPos.x = lanes[currentLaneIndex];
  newPos.y = currentPos.y + path;

  MoveTo* mt = MoveTo::create(time, newPos);
  mt->setTag(kMoveActionTag);
  runAction(mt);

// --- prepare move info
  std::pair<float, float> result;
  result.first = path;
  result.second = time;

  return result;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool EnemyCarNode::initPhysicsBody() {

//  int polygonPointCount = 0;
//  const Vec2*  polygonPointsArr = nullptr;
//  switch(type) {
//  case RT_small:
//    polygonPointCount = smallPointsCount;
//    polygonPointsArr = smallPoints;
//    break;
//  case RT_medium:
//    polygonPointCount = mediumPointsCount;
//    polygonPointsArr = mediumPoints;
//    break;
//  case RT_big:
//    // C6_D1(c6, "called for big");
//    polygonPointCount = bigPointsCount;
//    polygonPointsArr = bigPoints;
//    break;

//  default:
//    C6_C1(c6, "Bad call");
//    return false;
//  }

  PhysicsBody* physicsBody = PhysicsBody::createPolygon(yellowCarBodyPoints,
                                                        kYellowCarBodyPointsCount,
                                                        PhysicsMaterial(0.1f, 1.0f, 0.0f));
  if (physicsBody == nullptr) {
    C6_D1(c6, "Failed to create ph body");
    return false;
  }

  physicsBody->setDynamic(false);
  physicsBody->setCategoryBitmask(kEnemyCarCategoryBitmask);
  physicsBody->setCollisionBitmask(kPlayerCarCategoryBitmask);
  physicsBody->setContactTestBitmask(0xFFFFFFFF);

  addComponent(physicsBody);


  return true;
}


// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool EnemyCarNode::initSelf() {
  if (!initWithSpriteFrameName(kYellowCarFrameName)) {
    C6_C2(c6, "Failed to init with file ", kYellowCarFrameName);
    return false;    //
  }

  if (!initPhysicsBody()) {
    return false;
  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void EnemyCarNode::reactToBackAttack() {
  PhysicsBody* physicsBody = getPhysicsBody();
  if (physicsBody != nullptr) {
    physicsBody->setContactTestBitmask(0x00);// disable further contacts for this car
  }

  float laneDiff = lanes[0] - lanes[1];
  C6_D2(c6, "Lane difference is ", laneDiff);

  float outLane = 0;
  if (currentLaneIndex == 0) {
    outLane = lanes[0] + laneDiff;
  }
  else {
    outLane = lanes[1] - laneDiff;
  }

  C6_D2(c6, "Out position is ", outLane);


  Vec2 currentPos = getPosition();
  float velocity = (kSingleMoveDistance * currentGear )/ kSingleMoveInterval;
  float time = kTurnDistance / velocity;
  C6_D4(c6, "Here path = ", kTurnDistance, " time = ", time);

  Vec2 newPos;
  newPos.x = outLane;
  newPos.y = currentPos.y + kTurnDistance;

  MoveTo* mt = MoveTo::create(time, newPos);
  mt->setTag(kMoveActionTag);

  stopAllActionsByTag(kMoveActionTag);     // just in case
  runAction(mt);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void EnemyCarNode::setInitialPos(const int lane, const float value) {
  initialY = value;
  currentLaneIndex = lane;

  setPosition(Vec2(lanes[currentLaneIndex], initialY));
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void EnemyCarNode::setLanes(const float leftLaneX, const float rightLaneX) {
  currentLaneIndex = 1;
  lanes[0] = rightLaneX;
  lanes[1] = leftLaneX;

  if (initialY>=0) {
    setPosition(Vec2(lanes[currentLaneIndex], initialY));
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void EnemyCarNode::setRoadLength(const int inRoadLength) {
  roadLength = inRoadLength;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

