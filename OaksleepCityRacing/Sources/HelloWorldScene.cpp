#include "HelloWorldScene.h"

// SIzx Cats logger defines
#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

using namespace oaksleep_city_racing;
USING_NS_CC;

static const int kRoadTileZOrder = 10;
static const int kRoadTileLabelZOrder = 11;

static const int kCarZOrder = 11;

static const int kSingleMoveDistance = 400;
static const float kSingleMoveInterval = 2.0;

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

Scene* HelloWorldScene::createScene(std::shared_ptr<SixCatsLogger> inC6) {

  HelloWorldScene *pRet = new(std::nothrow) HelloWorldScene();
  if (pRet == nullptr) {
    return nullptr;
  }

  pRet->setLogger(inC6);

  if (pRet && pRet->init()) {
    pRet->autorelease();
    return pRet;
  }
  else {
    delete pRet;
    pRet = nullptr;
    return nullptr;
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void HelloWorldScene::doMoveCar() {
  Vec2 newPos = expectedCarPos;
  newPos.y = newPos.y + kSingleMoveDistance;

  MoveTo* mt = MoveTo::create(kSingleMoveInterval, newPos);
  expectedCarPos = newPos;

  CallFunc* cf = CallFunc::create([this]() {
    C6_D1(c6,"calling car to continue ride\n");

    doMoveCamera();

    doMoveCar();
  });

  car->runAction(Sequence::create(mt, cf, nullptr));
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void HelloWorldScene::doMoveCamera() {
  Camera* camera = getDefaultCamera();
  const Vec3 currentCameraPos = camera->getPosition3D();

  Vec3 newCameraPos = Vec3(currentCameraPos.x, currentCameraPos.y + kSingleMoveDistance,
                           currentCameraPos.z);

  MoveTo* cmt = MoveTo::create(kSingleMoveInterval, newCameraPos);
  camera->runAction(cmt);

  // const Size winSize = Director::getInstance()->getWinSize();
  // const float posDiff = fabs(currentCameraPos.x - expectedCarPos.x);
  // const float posTh = (winSize.width/2.0-150);
  // printf("%s: camera status: %4.2f vs %4.2f\n", __func__, posDiff, posTh);
  // if (posDiff>posTh) {
  //   Vec3 newCameraPos = Vec3(expectedCarPos.x, expectedCarPos.y, currentCameraPos.z);
  //   MoveTo* cmt = MoveTo::create(1, newCameraPos);
  //   camera->runAction(cmt);
  // }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool HelloWorldScene::init() {
  if ( !Scene::init() ) {
    return false;
  }

  if (!initBackgroundRoad()) {
    return false;
  }

  if (!initCar()) {
    return false;
  }

  if (!initKeyboardProcessing()) {
    return false;
  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool HelloWorldScene::initBackgroundRoad() {
  const Size currentWindowSize = getContentSize();
  Sprite* sprite = Sprite::create("road_tile_a.png");
  if (sprite == nullptr) {
    C6_C1(c6, "Failed to open 'road_tile_a.png'");
    return false;
  }

  const Size spriteSize = sprite->getContentSize();
  int xPos = currentWindowSize.width/2;
  int yPos = spriteSize.height/2;

  for (int i = 0; i<100; i++) {
    Sprite* tsp = Sprite::create("road_tile_a.png");

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

    yPos += spriteSize.height;
  }


  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool HelloWorldScene::initCar() {
  const Size currentWindowSize = getContentSize();
  car = Sprite::create("red_car.png");
  if (car == nullptr) {
    C6_C1(c6, "Failed to open 'red_car.png'");
    return false;
  }

  expectedCarPos = Vec2(currentWindowSize.width/2 + currentWindowSize.width/8, 100);
  car->setPosition(expectedCarPos);


  addChild(car, kCarZOrder);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool HelloWorldScene::initKeyboardProcessing() {
  // keyboard processing
  EventListenerKeyboard* sceneKeyboardListener = EventListenerKeyboard::create();
  sceneKeyboardListener->onKeyPressed = CC_CALLBACK_2(HelloWorldScene::onKeyPressedScene, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(sceneKeyboardListener, this);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void HelloWorldScene::onKeyPressedScene(EventKeyboard::KeyCode keyCode, Event *) {
  C6_D3(c6, "Key '", (int)keyCode, "' was pressed");

  if (EventKeyboard::KeyCode::KEY_BACKSPACE == keyCode) {
    C6_D1(c6, "That was KEY_BACKSPACE");
    // Director::getInstance()->popScene();
  }
  else if (EventKeyboard::KeyCode::KEY_K == keyCode) {
    doMoveCar();
  }
  else if (EventKeyboard::KeyCode::KEY_X == keyCode) {
    c6->d(__c6_MN__, "Need to get out.");

    // Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
