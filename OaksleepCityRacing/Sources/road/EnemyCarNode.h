#pragma once

#include <utility> // std::pair

#include "cocos2d.h"

#include "SixCatsLoggerLoggable.h"

#include "road/DataAdapters.h"

namespace oaksleep_city_racing {

class StaticElementsKeeper;

class EnemyCarNode : public cocos2d::Sprite, virtual public SixCatsLoggerLoggable {
public:
  static EnemyCarNode* create(std::shared_ptr<SixCatsLogger> c6);

  void setInitialPos(const int lane, const float value);
  void setRoadInfo(const RoadInfo& roadInfo);

  void reactToBackAttack();

  std::pair<float, float> doMove();

protected:
  EnemyCarNode();
  virtual ~EnemyCarNode();

  bool initSelf();
  bool initPhysicsBody();

//  bool isValidTarget;

  int currentLaneIndex;
  float lanes[3];// line positions start from right lane

  int currentGear;

  int roadLength;

  float initialY;
};

}
