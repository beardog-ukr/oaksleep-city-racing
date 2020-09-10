#include "PlayerCarNode.h"
using namespace oaksleep_city_racing;
#include "StaticElementsKeeper.h"

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

static const string kRedCarFrameName = "road_scene/red_car";

static const int kRedCarBodyPointsCount = 12;
static const Vec2 redCarBodyPoints[kRedCarBodyPointsCount] = {
  {.x =       14, .y =          -78     },
  {.x =       -14, .y =          -78     },
  {.x =       30, .y =          -67     },
  {.x =       -29, .y =          -67     },
  {.x =       32, .y =          -43     },
  {.x =       -32, .y =          -43     },
  {.x =       36, .y =          51      },
  {.x =       -36, .y =          51      },
  {.x =       28, .y =          69      },
  {.x =       -29, .y =          69      },
  {.x =       18, .y =          77      },
  {.x =       -16, .y =          77      },
};

static const int kPlayerCarCategoryBitmask = 0x01;
static const int kEnemyCarCategoryBitmask = 0x02;

// static const struct {
//   string north;
//   string south;
//   string east;
//   string west;
// } kMoveAnimationNames = {
//   .north = "actor_move_north",
//   .south = "actor_move_south",
//   .east = "actor_move_east",
//   .west = "actor_move_west"
// };

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

PlayerCarNode::PlayerCarNode() {
  currentGear = 1;
  currentLaneIndex = -1;// -1 indicates value was not initialized
  initialY = -1;

  staticElementsKeeper = nullptr;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

PlayerCarNode::~PlayerCarNode() {
  delete staticElementsKeeper;

  C6_F1(c6, "here");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

PlayerCarNode* PlayerCarNode::create(shared_ptr<SixCatsLogger> c6) {

  PlayerCarNode *pRet = new(std::nothrow) PlayerCarNode();
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

void PlayerCarNode::doChangeLane() {
  Vec2 currentPos = getPosition();
//    float path = roadLength - currentPos.y;
  float velocity = (kSingleMoveDistance * currentGear )/ kSingleMoveInterval;
  float time = kTurnDistance / velocity;
  C6_D4(c6, "Here path = ", kTurnDistance, " time = ", time);

  Vec2 newPos;
  newPos.x = lanes[currentLaneIndex];
  newPos.y = currentPos.y + kTurnDistance;

  MoveTo* mt = MoveTo::create(time, newPos);

  CallFunc* cf = CallFunc::create([this]() {
//    const pair<float, float>  moveResult =
    doMove();
//    staticElementsKeeper->doMove(moveResult);
  });

  Sequence* seq = Sequence::create(mt, cf, nullptr);
  seq->setTag(kMoveActionTag);

  // --- reset camera movement
  const pair<float, float> moveParameters = make_pair(kTurnDistance, time);
  staticElementsKeeper->doMove(moveParameters);

  stopAllActionsByTag(kMoveActionTag);     // just in case
  runAction(seq);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

std::pair<float, float> PlayerCarNode::doMove() {
  stopAllActionsByTag(kMoveActionTag); // just in case

//  int moveDistance = kSingleMoveDistance * currentGear;

//  Vec2 newPos = getPosition();
//  newPos.y = newPos.y + moveDistance;

//  CallFunc* cf = CallFunc::create([this]() {
//    doMove();
//  });

//  MoveTo* mt = MoveTo::create(kSingleMoveInterval, newPos);
////  mt->setTag(kMoveActionTag);
//  Sequence* seq =  Sequence::create(mt, cf, nullptr);
//  seq->setTag(kMoveActionTag);
//  runAction(seq);

//  // --- prepare move info
//  std::pair<float, float> result;
//  result.first = moveDistance;
//  result.second = kSingleMoveInterval;

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

  // --- reset camera movement
  staticElementsKeeper->doMove(result);

  return result;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool PlayerCarNode::initPhysicsBody() {

  PhysicsBody* physicsBody = PhysicsBody::createPolygon(redCarBodyPoints, kRedCarBodyPointsCount,
                                                        PhysicsMaterial(0.1f, 1.0f, 0.0f));
  if (physicsBody == nullptr) {
    C6_D1(c6, "Failed to create ph body");
    return false;
  }

  physicsBody->setDynamic(false);
  physicsBody->setCategoryBitmask(kPlayerCarCategoryBitmask);
  physicsBody->setCollisionBitmask(kEnemyCarCategoryBitmask);
  physicsBody->setContactTestBitmask(0xFFFFFFFF);

  addComponent(physicsBody);


  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool PlayerCarNode::initSelf() {
  if (!initWithSpriteFrameName(kRedCarFrameName)) {
    C6_C2(c6, "Failed to init with file ", kRedCarFrameName);
    return false;    //
  }

  setOpacity(50);

  if (!initPhysicsBody()) {
    return false;
  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool PlayerCarNode::makeTurnLeft() {
  if (currentLaneIndex == (kLineMax-1)) {
    return false;
  }
  //else
  currentLaneIndex++;
  doChangeLane();

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool PlayerCarNode::makeTurnRight() {
  if (currentLaneIndex == 0) {
    return false;
  }
  //else
  currentLaneIndex--;
  doChangeLane();

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool PlayerCarNode::setGearDown() {
  if (currentGear == 1) {
    return false;
  }

  currentGear = currentGear -1;
  doMove();

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool PlayerCarNode::setGearUp() {
  if (currentGear == kGearMax) {
    return false;
  }

  currentGear = currentGear +1;

  doMove();

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void PlayerCarNode::setInitialY(const float value) {
  initialY = value;

  if (currentLaneIndex>=0) {
    setPosition(Vec2(lanes[currentLaneIndex], initialY));
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void PlayerCarNode::setLanes(const float leftLaneX, const float rightLaneX) {
  currentLaneIndex = 1;
  lanes[0] = rightLaneX;
  lanes[1] = leftLaneX;

  if (initialY>=0) {
    setPosition(Vec2(lanes[currentLaneIndex], initialY));
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void PlayerCarNode::setRoadLength(const int inRoadLength) {
  roadLength = inRoadLength;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void PlayerCarNode::setStaticElementsKeeper(StaticElementsKeeper* keeper) {
  staticElementsKeeper = keeper;
}


// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

