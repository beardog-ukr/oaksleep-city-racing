#pragma once

#include <utility> // std::pair

#include "cocos2d.h"

#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class RoadNode : public cocos2d::Node, virtual public SixCatsLoggerLoggable {
public:
  static RoadNode* create(const cocos2d::Size& windowSize, std::shared_ptr<SixCatsLogger> c6);

  int getLength() const;

protected:
  RoadNode();
  virtual ~RoadNode();

  bool initSelf(const cocos2d::Size& windowSize);
  bool initRoad();
  bool initTerrain();

  int lengthTotal;
  cocos2d::Size windowSize;
};

}
