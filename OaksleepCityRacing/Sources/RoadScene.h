#pragma once

#include "cocos2d.h"
#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class RoadScene : public cocos2d::Scene,  virtual public SixCatsLoggerLoggable {
public:
  static cocos2d::Scene* createScene(std::shared_ptr<SixCatsLogger> c6);

protected:
  RoadScene();
  virtual ~RoadScene();

  cocos2d::Sprite* car;
  cocos2d::Vec2 expectedCarPos;

  void doMoveCar();
  void doMoveCamera();
  void doMoveStaticScreen();

  virtual bool init();
  bool initRoad();
  bool initTerrain();
  bool initCar();
  bool initKeyboardProcessing();
  void onKeyPressedScene(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *);

  static bool loadSpriteCache(std::shared_ptr<SixCatsLogger> c6);
  static void unloadSpriteCache();
};

}
