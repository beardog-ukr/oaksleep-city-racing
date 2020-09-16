#include "GameEndScene.h"


#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"
#include "ui/GameEndWindowNode.h"
#include "ui/MainWindowNode.h"


// Six Cats logger defines
#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

using namespace oaksleep_city_racing;
using namespace std;
USING_NS_CC;

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

static const struct {
  string background;
  string window;
  string buttonExit;
  string buttonPlay;
  string buttonSettings;
} kSpriteFileNames = {
  .background = "ocr_game/main/background",
  .window = "ocr_game/main/base_window",
  .buttonExit = "ocr_game/main/button_exit",
  .buttonPlay = "ocr_game/main/button_play",
  .buttonSettings = "ocr_game/main/button_settings",
};

static const string kPlistFileName = "ui_scene.plist";

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

GameEndScene::GameEndScene() {
  //
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

GameEndScene::~GameEndScene() {
  unloadSpriteCache();
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

Scene* GameEndScene::createScene(const bool victoryAchieved, std::shared_ptr<SixCatsLogger> inC6) {
  GameEndScene *pRet = new(std::nothrow) GameEndScene();

  do {
    if (pRet == nullptr) {
      break;
    }

    pRet->gameResult = victoryAchieved;

    if (!loadSpriteCache(inC6)) {
      break;
    }

    pRet->setLogger(inC6);

    if (pRet->initSelf()) {
      pRet->autorelease();
      return pRet; //successful exit
    }

  }while (false);

  delete pRet;
  pRet = nullptr;
  return nullptr;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool GameEndScene::initSelf() {
  if ( !Scene::init() ) {
    return false;
  }

  if (!initBackground()) {
    return false;
  }

  if (!initMenu()) {
    return false;
  }

  if (!initKeyboardProcessing()) {
    return false;
  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool GameEndScene::initKeyboardProcessing() {
  // keyboard processing
  EventListenerKeyboard* sceneKeyboardListener = EventListenerKeyboard::create();
  sceneKeyboardListener->onKeyPressed = CC_CALLBACK_2(GameEndScene::onKeyPressedScene, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(sceneKeyboardListener, this);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool GameEndScene::initBackground() {
  int roadLength = 0;

  const Size currentWindowSize = getContentSize();

  Sprite* sprite = Sprite::createWithSpriteFrameName(kSpriteFileNames.background);
  if (sprite == nullptr) {
    C6_C2(c6, "Failed to open ", kSpriteFileNames.background);
    return false;
  }

  const int tileHeight = sprite->getContentSize().height;
  const int tileWidth = sprite->getContentSize().width;

  float nextY = 0;
  while (nextY < currentWindowSize.height) {
    float nextX = 0;
    while(nextX< currentWindowSize.width) {
      Sprite* sprite = Sprite::createWithSpriteFrameName(kSpriteFileNames.background);
      if (sprite == nullptr) {
        C6_C2(c6, "Failed to open ", kSpriteFileNames.background);
        return false;
      }

      sprite->setAnchorPoint(Vec2(0,0));
      sprite->setPosition(nextX, nextY);
      addChild(sprite, kMainMenuSceneZO.background);

      nextX += tileWidth;
    }
    nextY += tileHeight;
  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool GameEndScene::initMenu() {
  const Size currentWindowSize = getContentSize();

  GameEndWindowNode* gewn = GameEndWindowNode::create(gameResult, c6);
  if (gewn == nullptr) {
    return false;
  }

  gewn->setPosition(currentWindowSize.width/2, currentWindowSize.height/2);
  addChild(gewn, kMainMenuSceneZO.window);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool GameEndScene::loadSpriteCache(std::shared_ptr<SixCatsLogger> c6) {
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

void GameEndScene::onKeyPressedScene(EventKeyboard::KeyCode keyCode, Event *) {
  C6_D3(c6, "Key '", (int)keyCode, "' was pressed");

  if (EventKeyboard::KeyCode::KEY_BACKSPACE == keyCode) {
    C6_D1(c6, "That was KEY_BACKSPACE");
  }
  else if (EventKeyboard::KeyCode::KEY_X == keyCode) {
    c6->d(__c6_MN__, "Need to get out (debug game end scene, x pressed).");

    // Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void GameEndScene::unloadSpriteCache() {
  // ActorNode::unloadAnimations();
  SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(kPlistFileName);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
