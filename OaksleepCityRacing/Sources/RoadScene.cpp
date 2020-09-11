#include "RoadScene.h"

#include "EnemyCarNode.h"
#include "PlayerCarNode.h"
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

//enum ZOrderValues {
//  kTerrainTileZOrder = 10,
//  kRoadTileZOrder = 11,
//  kRoadTileLabelZOrder = 12,

//  kCarZOrder = 20
//};

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
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

RoadScene::~RoadScene() {
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

  if (!initTerrain()) {
    return false;
  }

  const int roadLength = initRoad();
  if (roadLength == 0) {
    return false;
  }

  if (!initEnemyCars(roadLength)) {
    return false;
  }

  if (!initPlayerCar(roadLength)) {
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

bool RoadScene::initEnemyCars(const int roadLength) {
  const Size currentWindowSize = getContentSize();

  const float leftLane = currentWindowSize.width/2 - currentWindowSize.width/8;
  const float rightLane = currentWindowSize.width/2 + currentWindowSize.width/8;

  EnemyCarNode* enemyCar = EnemyCarNode::create(c6);
  if (enemyCar == nullptr) {
    return false;
  }

  enemyCar->setLanes(leftLane, rightLane);
  enemyCar->setInitialPos(0,100);
  enemyCar->setRoadLength(roadLength);
//  enemyCar->setStaticElementsKeeper(staticElementsKeeper);
  addChild(enemyCar, kRoadSceneZO.enemyCar);

  enemyCar->doMove();

  enemyCar = EnemyCarNode::create(c6);
  if (enemyCar == nullptr) {
    return false;
  }

  enemyCar->setLanes(leftLane, rightLane);
  enemyCar->setInitialPos(1,1000);
  enemyCar->setRoadLength(roadLength);
//  enemyCar->setStaticElementsKeeper(staticElementsKeeper);
  addChild(enemyCar, kRoadSceneZO.enemyCar);

//  enemyCar->doMove();
  return true;
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

bool RoadScene::initPlayerCar(const int roadLength) {
  StaticElementsKeeper* staticElementsKeeper = StaticElementsKeeper::create(this, c6);
  if (staticElementsKeeper == nullptr) {
    return false;
  }

  playerCar = PlayerCarNode::create(c6);
  if (playerCar == nullptr) {
    return false;
  }

  const Size currentWindowSize = getContentSize();
  const float leftLane = currentWindowSize.width/2 - currentWindowSize.width/8;
  const float rightLane = currentWindowSize.width/2 + currentWindowSize.width/8;
  playerCar->setLanes(leftLane, rightLane);
  playerCar->setInitialY(100);
  playerCar->setRoadLength(roadLength);
  playerCar->setStaticElementsKeeper(staticElementsKeeper);
  addChild(playerCar, kRoadSceneZO.playerCar);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

int RoadScene::initRoad() {
  int roadLength = 0;

  const Size currentWindowSize = getContentSize();
  Sprite* sprite = Sprite::createWithSpriteFrameName(kSpriteFileNames.road01);
  if (sprite == nullptr) {
    C6_C2(c6, "Failed to open ", kSpriteFileNames.road01);
    return 0;
  }

  const Size spriteSize = sprite->getContentSize();
  int xPos = currentWindowSize.width/2;
  int yPos = lround(spriteSize.height/2);

  for (int i = 0; i<100; i++) {
    Sprite* tsp = Sprite::createWithSpriteFrameName(kSpriteFileNames.road01);

    tsp->setPosition(Vec2(xPos, yPos));
    tsp->setOpacity(kElementsOpacity);

    addChild(tsp, kRoadSceneZO.road);

    if ((i%10==0)) {
      std::ostringstream oss;
      oss << "L:" << i;

      Label* label = Label::createWithTTF(oss.str(), "fonts/Marker Felt.ttf", 24);
      if (label == nullptr) {
        C6_C1(c6, "Failed to open 'fonts/Marker Felt.ttf'");
      }
      else {
        // position the label on the center of the screen
        label->setPosition(Vec2(currentWindowSize.width/2 - currentWindowSize.width/8, yPos));

        // add the label as a child to this layer
        this->addChild(label, kRoadSceneZO.roadLabel);
      }
    }

    yPos += (spriteSize.height -1);
    roadLength += spriteSize.height;// later it will not depend on number of iterations
  }

  return roadLength;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool RoadScene::initTerrain() {
  const Size currentWindowSize = getContentSize();
  Sprite* sprite = Sprite::createWithSpriteFrameName(kSpriteFileNames.terrain);
  if (sprite == nullptr) {
    C6_C2(c6, "Failed to open ", kSpriteFileNames.terrain);
    return false;
  }

  const Size spriteSize = sprite->getContentSize();
  int xPos = currentWindowSize.width - spriteSize.width/2;
  int yPos = 0;

  for (int i = 0; i<100; i++) {
    Sprite* tsp = Sprite::createWithSpriteFrameName(kSpriteFileNames.terrain);
    tsp->setPosition(Vec2(0, yPos));
    tsp->setOpacity(kElementsOpacity);
    addChild(tsp, kRoadSceneZO.terrain);

    tsp = Sprite::createWithSpriteFrameName(kSpriteFileNames.terrain);
    tsp->setPosition(Vec2(xPos, yPos));
    tsp->setOpacity(kElementsOpacity);
    addChild(tsp, kRoadSceneZO.terrain);

    yPos += (spriteSize.height -1);
  }

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
//    unschedule(CC_SCHEDULE_SELECTOR(RoadScene::doSingleMove));
  }
  else if (EventKeyboard::KeyCode::KEY_A == keyCode) {
    if (alreadyMoving) {
//      if (playerCar->setGearUp()) {
//        reevaluateMove();
//      }
      playerCar->setGearUp();
    }
    else {
      startMoving();
    }
  }
  else if (EventKeyboard::KeyCode::KEY_Z == keyCode) {
    if (alreadyMoving) {
      playerCar->setGearDown();
//      if (playerCar->setGearDown()) {
//        reevaluateMove();
//      }
    }
    else {
      startMoving();
    }
  }
  else if (EventKeyboard::KeyCode::KEY_LEFT_ARROW == keyCode) {
    if (alreadyMoving) {
      playerCar->makeTurnLeft();
//      const pair<float, float> turnResult = playerCar->doTurnLeft();
//      if (turnResult.first > 0) {
//        staticElementsKeeper->doMove(turnResult);
//      }
    }
  }
  else if (EventKeyboard::KeyCode::KEY_RIGHT_ARROW == keyCode) {
    if (alreadyMoving) {
      playerCar->makeTurnRight();
//      const pair<float, float> turnResult = playerCar->doTurnRight();
//      if (turnResult.first > 0) {
//        staticElementsKeeper->doMove(turnResult);
//      }
    }
  }
  else if (EventKeyboard::KeyCode::KEY_K == keyCode) {
    if (!alreadyMoving) {
      startMoving();
    }
  }
  else if (EventKeyboard::KeyCode::KEY_X == keyCode) {
    c6->d(__c6_MN__, "Need to get out.");

    // Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void RoadScene::startMoving() {
  alreadyMoving = true;

//  doSingleMove(0);
//  schedule(CC_SCHEDULE_SELECTOR(RoadScene::doSingleMove), 2.0, CC_REPEAT_FOREVER, 0);
  const pair<float, float> moveInfo = playerCar->doMove();
//  staticElementsKeeper->doMove(moveInfo);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void RoadScene::unloadSpriteCache() {
  // ActorNode::unloadAnimations();
  SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(kPlistFileName);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
