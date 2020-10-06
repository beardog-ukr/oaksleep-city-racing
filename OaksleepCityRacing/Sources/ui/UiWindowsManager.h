#pragma once

#include <list>
#include <utility> // std::pair

#include "cocos2d.h"

#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class UiWindowsManager : virtual public SixCatsLoggerLoggable {
public:
  UiWindowsManager(const cocos2d::Size& sceneSize, std::shared_ptr<SixCatsLogger> c6);
  virtual ~UiWindowsManager();

  void swapNodes(cocos2d::Node* previous, cocos2d::Node* replacement);
  void swapBack(cocos2d::Node* current);

protected:
  cocos2d::Vec2 firstPosition; // this node starts at "first" position;
  cocos2d::Vec2 secondPosition;// nodes will go to "second" position when they get replaced
  cocos2d::Vec2 normalPosition;// "normal" position for the node

  std::list<cocos2d::Node*> previousNodes;
};

}
