#pragma once

#include <utility> // std::pair

#include "cocos2d.h"

#include "SixCatsLoggerLoggable.h"

#include "DataAdapters.h"

namespace oaksleep_city_racing {

class RoadNode : public cocos2d::Node, virtual public SixCatsLoggerLoggable {
public:
  static RoadNode* create(const cocos2d::Size& windowSize, std::shared_ptr<SixCatsLogger> c6);

  // int getLength() const;
  void fillRoadInfo(RoadInfo& roadInfo);

protected:
  RoadNode();
  virtual ~RoadNode();

  bool initSelf(const cocos2d::Size& windowSize);
  bool initRoad();
  bool initRoadStart();
  bool initTerrain();

  int totalLength;
  int racingLength;
  float startPosition;

  cocos2d::Size windowSize;
};

}
