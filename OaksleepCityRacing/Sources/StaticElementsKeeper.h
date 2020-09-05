#pragma once

#include "cocos2d.h"

#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class StaticElementsKeeper : virtual public SixCatsLoggerLoggable {
public:
  StaticElementsKeeper();
  virtual ~StaticElementsKeeper();
  // static StaticElementsKeeper* create(std::shared_ptr<SixCatsLogger> c6);

//  void setRoadLength(const int roadLength);
  void setCamera(cocos2d::Camera* camera);
  void doMove(const std::pair<float, float> moveInfo);
  //const float moveDistance, const float timeInteval);

protected:
//  int roadLength;
  cocos2d::Camera* camera;
};

}
