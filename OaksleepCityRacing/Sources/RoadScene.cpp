#include "RoadScene.h"

#include "PlayerCarNode.h"
#include "StaticElementsKeeper.h"

// Six Cats logger defines
#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

using namespace oaksleep_city_racing;
using namespace std;
USING_NS_CC;

#include <cmath> // round

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

enum ZOrderValues {
  kTerrainTileZOrder = 10,
  kRoadTileZOrder = 11,
  kRoadTileLabelZOrder = 12,

  kCarZOrder = 20
};

static const struct {
  // string redCar;
  string road01;
  string road02;
  string road03;
  string terrain;
} kSpriteFileNames = {
  // .redCar = "road_scene/red_car",
  .road01 = "road_scene/road_tile_01",
  .road02 = "road_scene/road_tile_02",
  .road03 = "road_scene/road_tile_03",
  .terrain = "road_scene/soil_tile"
};

static const string kPlistFileName = "road_scene.plist";

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

RoadScene::RoadScene() {
  alreadyMoving = false;

  staticElementsKeeper = nullptr;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

RoadScene::~RoadScene() {
  delete staticElementsKeeper;

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

//void RoadScene::doSingleMove(float dt) {
//  C6_D2(c6, "Here, ", dt);

//  const pair<float, float> moveInfo = playerCar->doMove();
//  staticElementsKeeper->doMove(moveInfo);

////  schedule(CC_SCHEDULE_SELECTOR(RoadScene::doSingleMove), moveInfo.second, 0, 0);
//}

//void RoadScene::doMoveCar() {
//  Vec2 newPos = expectedCarPos;
//  newPos.y = newPos.y + kSingleMoveDistance;

//  MoveTo* mt = MoveTo::create(kSingleMoveInterval, newPos);
//  expectedCarPos = newPos;

//  CallFunc* cf = CallFunc::create([this]() {
//    C6_D1(c6,"calling car to continue ride\n");

//    doMoveCamera();

//    doMoveCar();
//  });

//  playerCar->runAction(Sequence::create(mt, cf, nullptr));
//}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

//void RoadScene::doMoveCamera() {
//  Camera* camera = getDefaultCamera();
//  const Vec3 currentCameraPos = camera->getPosition3D();

//  Vec3 newCameraPos = Vec3(currentCameraPos.x, currentCameraPos.y + kSingleMoveDistance,
//                           currentCameraPos.z);

//  MoveTo* cmt = MoveTo::create(kSingleMoveInterval, newCameraPos);
//  camera->runAction(cmt);
//}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

//void RoadScene::doMoveStaticScreen() {
//  Camera* camera = getDefaultCamera();
//  const Vec3 currentCameraPos = camera->getPosition3D();

//  Vec3 newCameraPos = Vec3(currentCameraPos.x, currentCameraPos.y + kSingleMoveDistance,
//                           currentCameraPos.z);

//  MoveTo* cmt = MoveTo::create(kSingleMoveInterval, newCameraPos);
//  camera->runAction(cmt);


//  Vec2 newPos = expectedCarPos;
//  newPos.y = newPos.y + kSingleMoveDistance;

//  MoveTo* mt = MoveTo::create(kSingleMoveInterval, newPos);
//  expectedCarPos = newPos;

//  CallFunc* cf = CallFunc::create([this]() {
//    C6_D1(c6,"calling car to continue ride\n");

//    doMoveCamera();

//    doMoveCar();
//  });

//  playerCar->runAction(Sequence::create(mt, cf, nullptr));
//}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool RoadScene::init() {
  if ( !Scene::init() ) {
    return false;
  }

  if (!initTerrain()) {
    return false;
  }

  const int roadLength = initRoad();
  if (roadLength == 0) {
    return false;
  }

  if (!initPlayerCar(roadLength)) {
    return false;
  }

  if (!initStaticElementsKeeper(roadLength)) {
    return false;
  }

  if (!initKeyboardProcessing()) {
    return false;
  }

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
  const Size currentWindowSize = getContentSize();
  // car = Sprite::createWithSpriteFrameName(kSpriteFileNames.redCar);
  // if (car == nullptr) {
  //   C6_C1(c6, "Failed to open 'red_car.png'");
  //   return false;
  // }

  // expectedCarPos = Vec2(currentWindowSize.width/2 + currentWindowSize.width/8, 100);
  // car->setPosition(expectedCarPos);

  // addChild(car, kCarZOrder);

  playerCar = PlayerCarNode::create(c6);
  if (playerCar == nullptr) {
    return false;
  }


  Vec2 expectedCarPos = Vec2(currentWindowSize.width/2 + currentWindowSize.width/8, 100);
  playerCar->setInitialPos(expectedCarPos);
  playerCar->setRoadLength(roadLength);
  addChild(playerCar, kCarZOrder);

  // playerCar->setCamera(getDefaultCamera());

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


    addChild(tsp, kRoadTileZOrder);

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
        this->addChild(label, kRoadTileLabelZOrder);
      }
    }

    yPos += (spriteSize.height -1);
    roadLength += spriteSize.height;// later it will not depend on number of iterations
  }

  return roadLength;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool RoadScene::initStaticElementsKeeper(const int roadLength) {
  staticElementsKeeper = new(nothrow) StaticElementsKeeper;
  if (staticElementsKeeper == nullptr) {
    return false;
  }

  staticElementsKeeper->setLogger(c6);
  staticElementsKeeper->setCamera(getDefaultCamera());

  return true;
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
    addChild(tsp, kTerrainTileZOrder);

    tsp = Sprite::createWithSpriteFrameName(kSpriteFileNames.terrain);
    tsp->setPosition(Vec2(xPos, yPos));
    addChild(tsp, kTerrainTileZOrder);


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

void RoadScene::onKeyPressedScene(EventKeyboard::KeyCode keyCode, Event *) {
  C6_D3(c6, "Key '", (int)keyCode, "' was pressed");

  if (EventKeyboard::KeyCode::KEY_BACKSPACE == keyCode) {
    C6_D1(c6, "That was KEY_BACKSPACE, it stops car");
//    unschedule(CC_SCHEDULE_SELECTOR(RoadScene::doSingleMove));
  }
  else if (EventKeyboard::KeyCode::KEY_A == keyCode) {
    if (alreadyMoving) {
      if (playerCar->setGearUp()) {
        reevaluateMove();
      }
    }
    else {
      startMoving();
    }
  }
  else if (EventKeyboard::KeyCode::KEY_Z == keyCode) {
    if (alreadyMoving) {
      if (playerCar->setGearDown()) {
        reevaluateMove();
      }
    }
    else {
      startMoving();
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

void RoadScene::reevaluateMove() {
//  unschedule(CC_SCHEDULE_SELECTOR(RoadScene::doSingleMove));
//  doSingleMove(0);
//  schedule(CC_SCHEDULE_SELECTOR(RoadScene::doSingleMove), 2.0, CC_REPEAT_FOREVER, 0);

  const pair<float, float> moveInfo = playerCar->doMove();
  staticElementsKeeper->doMove(moveInfo);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void RoadScene::startMoving() {
  alreadyMoving = true;

//  doSingleMove(0);
//  schedule(CC_SCHEDULE_SELECTOR(RoadScene::doSingleMove), 2.0, CC_REPEAT_FOREVER, 0);
  const pair<float, float> moveInfo = playerCar->doMove();
  staticElementsKeeper->doMove(moveInfo);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void RoadScene::unloadSpriteCache() {
  // ActorNode::unloadAnimations();
  SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(kPlistFileName);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
