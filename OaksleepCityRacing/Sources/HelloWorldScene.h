#pragma once

#include "cocos2d.h"
#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class HelloWorldScene : public cocos2d::Scene,
  virtual public SixCatsLoggerLoggable {
public:
  static cocos2d::Scene* createScene();

  virtual bool init();

  // a selector callback
  void menuCloseCallback(cocos2d::Ref* pSender);

  static cocos2d::Scene* createScene(std::shared_ptr<SixCatsLogger> c6);

protected:
  bool initKeyboardProcessing();
  void onKeyPressedScene(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *);
};

}
