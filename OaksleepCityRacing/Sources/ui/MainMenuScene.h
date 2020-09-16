#pragma once

#include "cocos2d.h"
#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class MainMenuScene : public cocos2d::Scene,  virtual public SixCatsLoggerLoggable {
public:
  static cocos2d::Scene* createScene(std::shared_ptr<SixCatsLogger> c6);

protected:
  MainMenuScene();
  virtual ~MainMenuScene();

  virtual bool init();
  bool initBackground();
  bool initMenu();
  bool initKeyboardProcessing();

  void onKeyPressedScene(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *);

  static bool loadSpriteCache(std::shared_ptr<SixCatsLogger> c6);
  static void unloadSpriteCache();
};

}
