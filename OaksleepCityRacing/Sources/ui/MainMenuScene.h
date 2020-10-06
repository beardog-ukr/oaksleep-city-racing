#pragma once

#include <memory> // shared_ptr

#include "cocos2d.h"
#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class UiWindowsManager;

class MainMenuScene : public cocos2d::Scene,  virtual public SixCatsLoggerLoggable {
public:
  static cocos2d::Scene* createScene(std::shared_ptr<SixCatsLogger> c6);

protected:
  MainMenuScene();
  virtual ~MainMenuScene();

  virtual bool init();
  bool initBackground();
  bool initKeyboardProcessing();
  bool initMenu(const float scaleFactor);
  bool initSettingsMenu(const float scaleFactor);

  std::shared_ptr<UiWindowsManager>  uiWindowsManager;

  void onKeyPressedScene(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *);

  static bool loadSpriteCache(std::shared_ptr<SixCatsLogger> c6);
  static void unloadSpriteCache();
};

}
