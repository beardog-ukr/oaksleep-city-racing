#include "RoadScene.h"

#include "EnemyCarNode.h"
#include "EnemyCarsKeeper.h"
#include "PlayerCarNode.h"
#include "RoadNode.h"
#include "StaticElementsKeeper.h"

#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"

// Six Cats logger defines
#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

using namespace oaksleep_city_racing;
using namespace std;
USING_NS_CC;

#include <cmath> // round

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

static const struct {
  // string redCar;
  string road01;
  string road02;
  string road03;
  string terrain;
} kSpriteFileNames = {
  .road01 = "ocr_game/terrain/road_tile_01",
  .road02 = "ocr_game/terrain/road_tile_02",
  .road03 = "ocr_game/terrain/road_tile_03",
  .terrain = "ocr_game/terrain/soil_tile"
};

static const int kElementsOpacity = 255; //must be 255 in release mode; this value is used
                                         //during physics world deburring

static const string kPlistFileName = "road_scene.plist";

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

RoadScene::RoadScene() {
  alreadyMoving = false;
  enemyCarsKeeper = nullptr;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

RoadScene::~RoadScene() {
  delete enemyCarsKeeper;

  unloadSpriteCache();
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

Scene* RoadScene::createScene(std::shared_ptr<SixCatsLogger> inC6) {
  RoadScene *pRet = new(std::nothrow) RoadScene();

  do {
    if (pRet == nullptr) {
      break;
    }

    if (!loadSpriteCache(inC6)) {
      break;
    }

    pRet->setLogger(inC6);

    if (pRet->init()) {
      pRet->autorelease();
      return pRet; //successful exit
    }

  }while (false);

  // else {
  delete pRet;
  pRet = nullptr;
  return nullptr;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool RoadScene::init() {
  if ( !Scene::initWithPhysics() ) {
    return false;
  }

  PhysicsWorld* phw = getPhysicsWorld();
  if (phw == nullptr) {
    C6_D1(c6, "Failed to get ph world");
    return false;
  }
  //else
  phw->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

  // const int roadLength = initRoad();
  if (!initRoad() ) {
    return false;
  }

  if (!initEnemyCars()) {
    return false;
  }

  if (!initPlayerCar()) {
    return false;
  }

  if (!initKeyboardProcessing()) {
    return false;
  }

  EventListenerPhysicsContact* contactListener = EventListenerPhysicsContact::create();
  contactListener->onContactBegin = CC_CALLBACK_1(RoadScene::onContactBegin, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);


  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool RoadScene::initEnemyCars() {

  enemyCarsKeeper = EnemyCarsKeeper::create(c6);
  if (enemyCarsKeeper == nullptr) {
    return false;
  }

  RoadInfo roadInfo;
  roadNode->fillRoadInfo(roadInfo);

  roadInfo.roadLength += getContentSize().height; // to ensule enemy cars go behind sceeen

  return enemyCarsKeeper->generateCars(roadInfo.roadLength, this);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool RoadScene::initKeyboardProcessing() {
  // keyboard processing
  EventListenerKeyboard* sceneKeyboardListener = EventListenerKeyboard::create();
  sceneKeyboardListener->onKeyPressed = CC_CALLBACK_2(RoadScene::onKeyPressedScene, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(sceneKeyboardListener, this);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool RoadScene::initPlayerCar() {
  StaticElementsKeeper* staticElementsKeeper = StaticElementsKeeper::create(this, c6);
  if (staticElementsKeeper == nullptr) {
    return false;
  }

  playerCar = PlayerCarNode::create(c6);
  if (playerCar == nullptr) {
    return false;
  }

  playerCar->setStaticElementsKeeper(staticElementsKeeper);

  RoadInfo roadInfo;
  roadNode->fillRoadInfo(roadInfo);

  const Size currentWindowSize = getContentSize();
  roadInfo.leftLaneX = currentWindowSize.width/2 - currentWindowSize.width/8;
  roadInfo.rightLaneX = currentWindowSize.width/2 + currentWindowSize.width/8;

  playerCar->setRoadInfo(roadInfo);

  addChild(playerCar, kRoadSceneZO.playerCar);

  // --- starting scene effect
  const float effectDuration = playerCar->doMoveToStart(currentWindowSize.height);
  schedule(CC_SCHEDULE_SELECTOR(RoadScene::startMoving), effectDuration, 0, 0);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool RoadScene::initRoad() {
  roadNode = RoadNode::create(getContentSize(), c6);
  if (roadNode == nullptr) {
    return false;
  }

  roadNode->setAnchorPoint(Vec2(0,0));
  addChild(roadNode, kRoadSceneZO.terrain);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool RoadScene::loadSpriteCache(std::shared_ptr<SixCatsLogger> c6) {
  SpriteFrameCache* const sfc = SpriteFrameCache::getInstance();

  sfc->addSpriteFramesWithFile(kPlistFileName);
  if (!sfc->isSpriteFramesWithFileLoaded(kPlistFileName)) {
    C6_C2(c6, "Failed to find ", kPlistFileName);
    return false;
  }

  // AnimationCache * const ac = AnimationCache::getInstance();
  // ac->addAnimationsWithFile(kAnimationsPlistFileName);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool RoadScene::onContactBegin(PhysicsContact& contact) {
  C6_D1(c6, "contact");

  Node* nodeA = contact.getShapeA()->getBody()->getNode();
  Node* nodeB = contact.getShapeB()->getBody()->getNode();
  if((nodeA == nullptr)||(nodeB ==nullptr)) {
    C6_I1(c6, "Bad node");
    return true;
  }

  EnemyCarNode* ecn = nullptr;
  if (nodeA->getTag() == PlayerCarNode::kTag) {
    ecn = dynamic_cast<EnemyCarNode*>(nodeB);
  }
  else {
    ecn = dynamic_cast<EnemyCarNode*>(nodeA);
  }

  if (ecn == nullptr) {
    C6_D1(c6, "Failed to get enemy in contact");
    return true;
  }

  ecn->reactToBackAttack();

  playerCar->reactToEnemyContact();

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void RoadScene::onKeyPressedScene(EventKeyboard::KeyCode keyCode, Event *) {
  C6_D3(c6, "Key '", (int)keyCode, "' was pressed");

  if (EventKeyboard::KeyCode::KEY_BACKSPACE == keyCode) {
    C6_D1(c6, "That was KEY_BACKSPACE, it stops car");
  }
  else if (EventKeyboard::KeyCode::KEY_A == keyCode) {
    if (alreadyMoving) {
      playerCar->setGearUp();
    }
  }
  else if (EventKeyboard::KeyCode::KEY_Z == keyCode) {
    if (alreadyMoving) {
      playerCar->setGearDown();
    }
  }
  else if (EventKeyboard::KeyCode::KEY_LEFT_ARROW == keyCode) {
    if (alreadyMoving) {
      playerCar->makeTurnLeft();
    }
  }
  else if (EventKeyboard::KeyCode::KEY_RIGHT_ARROW == keyCode) {
    if (alreadyMoving) {
      playerCar->makeTurnRight();
    }
  }
  else if (EventKeyboard::KeyCode::KEY_X == keyCode) {
    c6->d(__c6_MN__, "Need to get out (debug, x pressed).");

    // Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void RoadScene::startMoving(float) {
  C6_D1(c6, "Here");

  alreadyMoving = true;

  playerCar->doMove();

  enemyCarsKeeper->startCars();
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void RoadScene::unloadSpriteCache() {
  // ActorNode::unloadAnimations();
  SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(kPlistFileName);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
