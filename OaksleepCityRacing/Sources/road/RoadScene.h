#pragma once

#include "cocos2d.h"
#include "SixCatsLoggerLoggable.h"

#include "road/DataAdapters.h"

namespace oaksleep_city_racing {

class RoadNode;
class PlayerCarNode;
class EnemyCarsKeeper;

class RoadScene : public cocos2d::Scene,  virtual public SixCatsLoggerLoggable {
public:
  static cocos2d::Scene* createScene(std::shared_ptr<SixCatsLogger> c6);

protected:
  RoadScene();
  virtual ~RoadScene();

  PlayerCarNode* playerCar;

  virtual bool init();
  bool initRoad();
  bool initEnemyCars(const RoadInfo& roadInfo);
  bool initPlayerCar(const RoadInfo &roadInfo);
  bool initKeyboardProcessing();

  void onKeyPressedScene(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *);

  bool alreadyMoving;
  void startMoving(float unusedValue);
  void doSingleMove(float dt);// here parameter is not used, but is required by CC_SCHEDULE_SELECTOR

  bool onContactBegin(cocos2d::PhysicsContact& contact);

  static bool loadSpriteCache(std::shared_ptr<SixCatsLogger> c6);
  static void unloadSpriteCache();

  RoadNode* roadNode;
  EnemyCarsKeeper* enemyCarsKeeper;
};

}
