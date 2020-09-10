#pragma once

#include "cocos2d.h"
#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class PlayerCarNode;
// class EnemyCarNode;

class RoadScene : public cocos2d::Scene,  virtual public SixCatsLoggerLoggable {
public:
  static cocos2d::Scene* createScene(std::shared_ptr<SixCatsLogger> c6);

protected:
  RoadScene();
  virtual ~RoadScene();

//  StaticElementsKeeper* staticElementsKeeper;
  PlayerCarNode* playerCar;

  virtual bool init();
  int initRoad();
  bool initTerrain();
  bool initEnemyCars(const int roadLength);
  bool initPlayerCar(const int roadLength);
  bool initKeyboardProcessing();

  void onKeyPressedScene(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *);

  bool alreadyMoving;
  void startMoving();
  void doSingleMove(float dt);// here parameter is not used, but is required by CC_SCHEDULE_SELECTOR

  bool onContactBegin(cocos2d::PhysicsContact& contact);

  static bool loadSpriteCache(std::shared_ptr<SixCatsLogger> c6);
  static void unloadSpriteCache();
};

}
