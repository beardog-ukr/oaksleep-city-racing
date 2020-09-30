#include "PlayerCarNode.h"
using namespace oaksleep_city_racing;
#include "road/StaticElementsKeeper.h"
#include "ui/GameEndScene.h"

#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

#include "AudioEngine.h"

USING_NS_CC;
using namespace std;

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

static const int kGearMax = 4;
static const int kLineMax = 2;

static const int kMoveActionTag = 222;
static const int kMoveSoundActionTag = 223;

static const int kSingleMoveDistance = 400;
static const float kSingleMoveInterval = 2.0;
const int PlayerCarNode::kTurnDistance = 200;

static const string kRedCarFrameName = "ocr_game/cars/red_car_5";

static const int kRedCarBodyPointsCount = 12;
static const Vec2 redCarBodyPoints[kRedCarBodyPointsCount] = {
  {.x =  14, .y = -78     },
  {.x = -14, .y = -78     },
  {.x =  30, .y = -67     },
  {.x = -29, .y = -67     },
  {.x =  32, .y = -43     },
  {.x = -32, .y = -43     },
  {.x =  36, .y =  51      },
  {.x = -36, .y =  51      },
  {.x =  28, .y =  69      },
  {.x = -29, .y =  69      },
  {.x =  18, .y =  77      },
  {.x = -16, .y =  77      },
};

static const int kPlayerCarCategoryBitmask = 0x01;
static const int kEnemyCarCategoryBitmask = 0x02;

const int PlayerCarNode::kTag = 20;

const int kLifesMax = 5;

enum GearSwitchType : int {
  kGearRemainsSame = 1,
  kGearGetsDown = 2,
  kGearGetsUp = 3
};
static const int kGearSoundTh = 2;

static const string kEngineSoundSlowFN = "sounds/engine_slow.mp3";
static const float kEngineSoundSlowDuration = 2.37;
static const string kEngineSoundFastFN = "sounds/engine_fast.mp3";
static const float kEngineSoundFastDuration = 2.44;
static const string kEngineSoundStartFN = "sounds/engine_start.mp3";

static const int kCrashSoundsMaxId = 2;
static const string kCrashSounds[kCrashSoundsMaxId] = {
  "sounds/car_crash_01.mp3", "sounds/car_crash_02.mp3"};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

PlayerCarNode::PlayerCarNode() {
  currentGear = 1;

  currentLaneIndex = -1;// -1 indicates value was not initialized
  laneChangeInProgress = false;

  initialY = -1;

  staticElementsKeeper = nullptr;

  lifesCounter = kLifesMax-1;

  currentMoveSoundId = 0;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

PlayerCarNode::~PlayerCarNode() {
  delete staticElementsKeeper;

  if (currentMoveSoundId!=0) {
    AudioEngine::stop(currentMoveSoundId);
  }

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
    this->laneChangeInProgress = false;
    doMove();
  });

  Sequence* seq = Sequence::create(mt, cf, nullptr);
  seq->setTag(kMoveActionTag);

  // --- reset camera movement
  const pair<float, float> moveParameters = make_pair(kTurnDistance, time);
  staticElementsKeeper->doMove(moveParameters);

  laneChangeInProgress = true;
  stopAllActionsByTag(kMoveActionTag);     // just in case
  runAction(seq);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void PlayerCarNode::doDie() {
  stopAllActionsByTag(kMoveActionTag);

  currentGear = 1;

  // --- last move action
  Vec2 currentPos = getPosition();
  float path = kTurnDistance;
  float velocity = (kSingleMoveDistance * currentGear )/ kSingleMoveInterval;
  float time = path / velocity;
  C6_T4(c6, "Here path = ", path, " time = ", time);

  Vec2 newPos;
  newPos.x = lanes[currentLaneIndex];
  newPos.y = currentPos.y + path;

  MoveTo* mt = MoveTo::create(time, newPos);
  mt->setTag(kMoveActionTag);
  runAction(mt);

  // --- other actions that indicate dying
  RotateBy* rb = RotateBy::create(time, 20);

  CallFunc* cf = CallFunc::create([this]() {
    Scene* ges = GameEndScene::createScene(false, c6);
    if (ges == nullptr) {
      return;
    }
    //
    Director::getInstance()->replaceScene(ges);
  });

  DelayTime* dta =  DelayTime::create(time);

  Sequence* seq = Sequence::create(rb, dta, cf, nullptr);
  seq->setTag(kMoveActionTag);

  runAction(seq);

  // --- prepare move info
  std::pair<float, float> result;
  result.first = path;
  result.second = time*2; // slightly slower camera effect

  // --- reset camera movement
  staticElementsKeeper->doMove(result);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

std::pair<float, float> PlayerCarNode::doMove() {
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
//  mt->setTag(kMoveActionTag);
//  runAction(mt);

  CallFunc* cf = CallFunc::create([this]() {
    Scene* ges = GameEndScene::createScene(true, c6);
    if (ges == nullptr) {
      return;
    }
    //
    Director::getInstance()->replaceScene(ges);
  });

  Sequence* seq = Sequence::create(mt, cf, nullptr);
  seq->setTag(kMoveActionTag);
  runAction(seq);

  // --- sound
  reevaluateMoveSound(kGearRemainsSame);

  // --- prepare move info
  std::pair<float, float> result;
  result.first = path;
  result.second = time;

  // --- reset camera movement
  staticElementsKeeper->doMove(result);

  return result;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

float PlayerCarNode::doMoveToStart(const int windowHeight) {
  const int kEffectDuration = 2;

  const Size carSize = getContentSize();
  const int bottomDistance = floor(carSize.height /10); // take 10% of car height
  staticElementsKeeper->doMove(make_pair( windowHeight/2 - carSize.height - bottomDistance,
                                          kEffectDuration));

  //--- add starting sound
  int startingSoundId = AudioEngine::play2d(kEngineSoundStartFN);
  C6_D2(c6, "Started sound effect as ", startingSoundId);
  if (startingSoundId == AudioEngine::INVALID_AUDIO_ID) {
    C6_D1(c6, "audio id invalid");
  }

  CallFunc* cf = CallFunc::create([startingSoundId]() {
    AudioEngine::stop(startingSoundId);
  });
  Sequence* seq = Sequence::create(DelayTime::create(kEffectDuration), cf, nullptr);
  runAction(seq);

  //--- finally
  return kEffectDuration;
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

//  setOpacity(50);
  setTag(kTag);

  if (!initPhysicsBody()) {
    return false;
  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool PlayerCarNode::isTurning() {
  return laneChangeInProgress;
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

void PlayerCarNode::reactToEnemyContact() {
  const int csfnid = RandomHelper::random_int((int)0, kCrashSoundsMaxId-1);
  int startingSoundId = AudioEngine::play2d(kCrashSounds[csfnid]);
  if (startingSoundId == AudioEngine::INVALID_AUDIO_ID) {
    C6_D2(c6, "audio id invalid, started ", kCrashSounds[csfnid] );
  }

  lifesCounter--;
  staticElementsKeeper->setLifesCounter(lifesCounter);
  if (lifesCounter == 0) {
    doDie();
    return;
  }

  if (laneChangeInProgress) {
    C6_D1(c6, "Conflict ignored because of lanes changing in process");
    return;
  }

  currentGear = 1;
  reevaluateMoveSound(kGearGetsDown);
  doMove();
  staticElementsKeeper->setGearIndicator(currentGear);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void PlayerCarNode::reevaluateMoveSound(const int soundChange) {
  string soundFileName;
  float soundDuration;
  bool stopPrevious = false;
  switch (soundChange) {
  case kGearRemainsSame: {
    if (currentGear<=2) {
      soundFileName = kEngineSoundSlowFN;
      soundDuration = kEngineSoundSlowDuration;
    }
    else {
      soundFileName = kEngineSoundFastFN;
      soundDuration = kEngineSoundFastDuration;
    }
    break;
  }

  case kGearGetsDown: {
    soundFileName = kEngineSoundSlowFN;
    soundDuration = kEngineSoundSlowDuration;
    stopPrevious = true;
    break;
  }

  case kGearGetsUp: {
    soundFileName = kEngineSoundFastFN;
    soundDuration = kEngineSoundFastDuration;
    stopPrevious = true;
    break;
  }

  default:
    C6_D1(c6, "ERROR: unexpected value of soundChange");
    soundFileName = kEngineSoundSlowFN;
    soundDuration = kEngineSoundSlowDuration;
  }

  if (stopPrevious) {
    AudioEngine::stop(currentMoveSoundId);
    currentMoveSoundId = 0;
    stopAllActionsByTag(kMoveSoundActionTag);
  }

  //--- add starting sound
  if (currentMoveSoundId == 0) {
    CallFunc* cf = CallFunc::create([this, soundFileName]() {
      this->currentMoveSoundId = AudioEngine::play2d(soundFileName);
      //    C6_D2(c6, "Started sound effect as ", startingSoundId);
      if (this->currentMoveSoundId == AudioEngine::INVALID_AUDIO_ID) {
        C6_D2(c6, "audio id invalid for ", soundFileName);
        //        break;
      }
    });

    Sequence* seq = Sequence::create(cf, DelayTime::create(soundDuration), nullptr);
    RepeatForever* rf = RepeatForever::create(seq);
    rf->setTag(kMoveSoundActionTag);
    runAction(rf);
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool PlayerCarNode::setGearDown() {
  if (currentGear == 1) {
    return false;
  }

  if (currentGear == (kGearSoundTh+1)) {
    reevaluateMoveSound(kGearGetsDown);
  }

  currentGear = currentGear -1;
  doMove();
  staticElementsKeeper->setGearIndicator(currentGear);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool PlayerCarNode::setGearUp() {
  if (currentGear == kGearMax) {
    return false;
  }

  if (currentGear == (kGearSoundTh)) {
    reevaluateMoveSound(kGearGetsUp);
  }

  currentGear = currentGear +1;
  doMove();
  staticElementsKeeper->setGearIndicator(currentGear);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void PlayerCarNode::setRoadInfo(const RoadInfo& roadInfo) {
  // --- lanes
  currentLaneIndex = 1;
  lanes[0] = roadInfo.rightLaneX;
  lanes[1] = roadInfo.leftLaneX;

  // --- start position
  initialY = roadInfo.startPosition;

  const Size carSize = getContentSize();
  setPosition(Vec2(lanes[currentLaneIndex], initialY - carSize.height/2));

  staticElementsKeeper->setYPosition(roadInfo.startPosition);

  // --- road length
  roadLength = roadInfo.roadLength;

}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void PlayerCarNode::setStaticElementsKeeper(StaticElementsKeeper* keeper) {
  staticElementsKeeper = keeper;
  staticElementsKeeper->setLifesCounter(lifesCounter);
}


// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

