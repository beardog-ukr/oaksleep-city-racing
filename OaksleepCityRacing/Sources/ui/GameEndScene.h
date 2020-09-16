#pragma once

#include "cocos2d.h"
#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class GameEndScene : public cocos2d::Scene,  virtual public SixCatsLoggerLoggable {
public:
  static cocos2d::Scene* createScene(const bool victoryAchieved, std::shared_ptr<SixCatsLogger> c6);

protected:
  GameEndScene();
  virtual ~GameEndScene();

  bool initSelf();
  bool initBackground();
  bool initMenu();
  bool initKeyboardProcessing();

  void onKeyPressedScene(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *);

  static bool loadSpriteCache(std::shared_ptr<SixCatsLogger> c6);
  static void unloadSpriteCache();

  bool gameResult;
};

}
