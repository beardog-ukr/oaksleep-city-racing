#pragma once

#include <utility> // std::pair

#include "cocos2d.h"

#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class MainWindowNode : public cocos2d::Sprite, virtual public SixCatsLoggerLoggable {
public:
  static MainWindowNode* create(std::shared_ptr<SixCatsLogger> c6);

protected:
  MainWindowNode();
  virtual ~MainWindowNode();

  void mcExit(cocos2d::Ref *pSender);
  void mcNewGame(cocos2d::Ref *pSender);
  void mcSettings(cocos2d::Ref *pSender);

  bool initSelf();
  bool initMainLabel();
  bool initMenuButtons();

};

}
