#include "EnemyCarNode.h"
using namespace oaksleep_city_racing;

#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"

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

static const string kYellowCarFrameName = "ocr/cars/yellow_car";

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
  C6_T4(c6, "Here path = ", path, " time = ", time);

  Vec2 newPos;
  newPos.x = lanes[currentLaneIndex];
  newPos.y = currentPos.y + path;

  MoveTo* mt = MoveTo::create(time, newPos);
//  mt->setTag(kMoveActionTag);

  CallFunc* cf = CallFunc::create([this]() {
    PhysicsBody* physicsBody = this->getPhysicsBody();
    if (physicsBody != nullptr) {
      physicsBody->setContactTestBitmask(0x00);  // disable further contacts for this car
    }
  });

  Sequence* seq = Sequence::create(mt, cf, RemoveSelf::create(), nullptr);
  seq->setTag(kMoveActionTag);

  runAction(seq);

// --- prepare move info
  std::pair<float, float> result;
  result.first = path;
  result.second = time;

  return result;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool EnemyCarNode::initPhysicsBody() {

  const int kBodyPointsCount = 10;
  const Vec2 kNormalizedBodyPoints[kBodyPointsCount] ={
    Vec2(0.09874990582466125, 1.6862502098083496)  ,
            Vec2(0.21874994039535522, 1.8612501621246338)  ,
            Vec2(0.38624992966651917, 1.9212502241134644)  ,
            Vec2(0.5987498760223389,  1.9212502241134644)  ,
            Vec2(0.7937499284744263,  1.851250171661377 ) ,
            Vec2(0.903749942779541, 1.6862502098083496  ),
            Vec2(0.9000000357627869, 0.22500000894069672)  ,
    Vec2(0.75, 0.07500000298023224        ),
    Vec2(0.25, 0.07500000298023224        ),
Vec2(0.09624981880187988, 0.2412499487400055)
  };
  //= {
//    {  .x = 0.09874990582466125, .y = 1.6862502098083496  },
//    {  .x = 0.21874994039535522, .y = 1.8612501621246338  },
//    {  .x = 0.38624992966651917, .y = 1.9212502241134644  },
//    {  .x = 0.5987498760223389, .y = 1.9212502241134644  },
//    {  .x = 0.7937499284744263, .y = 1.851250171661377  },
//    {  .x = 0.903749942779541, .y = 1.6862502098083496  },
//    {  .x = 0.9000000357627869, .y = 0.22500000894069672  },
//    {  .x = 0.75, .y = 0.07500000298023224        },
//    {  .x = 0.25, .y = 0.07500000298023224        },
//    {  .x = 0.09624981880187988, .y = 0.2412499487400055  }
//  };
  Vec2 bodyPoints[kBodyPointsCount];

  const Size cs = getContentSize();
  const float halfWidth = cs.width/2;
  const float halfHeight = cs.height/2;
  for (int i = 0; i< kBodyPointsCount; i++) {
    bodyPoints[i].x = kNormalizedBodyPoints[i].x*cs.width - halfWidth;
    bodyPoints[i].y = kNormalizedBodyPoints[i].y*cs.width - halfHeight;
  }

  PhysicsBody* physicsBody = PhysicsBody::createPolygon(bodyPoints, kBodyPointsCount,
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

  setOpacity(kElementsOpacity);

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

void EnemyCarNode::setRoadInfo(const RoadInfo& roadInfo) {
  roadLength = roadInfo.enemyFinishPoint;

  currentLaneIndex = 1;
  lanes[0] = roadInfo.rightLaneX;
  lanes[1] = roadInfo.leftLaneX;

  if (initialY>=0) {
    setPosition(Vec2(lanes[currentLaneIndex], initialY));
  }

  setScale(roadInfo.screenScaleFactor);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

