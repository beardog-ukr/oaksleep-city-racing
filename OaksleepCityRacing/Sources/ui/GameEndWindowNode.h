#pragma once

#include <utility> // std::pair

#include "cocos2d.h"

#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class GameEndWindowNode : public cocos2d::Sprite, virtual public SixCatsLoggerLoggable {
public:
  static GameEndWindowNode* create(const bool victoryAchieved, std::shared_ptr<SixCatsLogger> c6);

protected:
  GameEndWindowNode();
  virtual ~GameEndWindowNode();

  void mcClose(cocos2d::Ref *pSender);
  void mcReplay(cocos2d::Ref *pSender);

  bool initSelf();
  bool initMainLabel();
  bool initMenuButtons();

  bool victoryMode;
};

}
