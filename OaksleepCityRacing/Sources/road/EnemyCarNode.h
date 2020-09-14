#pragma once

#include <utility> // std::pair

#include "cocos2d.h"

#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class StaticElementsKeeper;

class EnemyCarNode : public cocos2d::Sprite, virtual public SixCatsLoggerLoggable {
public:
  static EnemyCarNode* create(std::shared_ptr<SixCatsLogger> c6);

  void setInitialPos(const int lane, const float value);
  void setLanes(const float leftLaneX, const float rightLaneX);
  void setRoadLength(const int roadLength);

  void reactToBackAttack();

  std::pair<float, float> doMove();

protected:
  EnemyCarNode();
  virtual ~EnemyCarNode();

  bool initSelf();
  bool initPhysicsBody();

  int currentLaneIndex;
  float lanes[3];// line positions start from right lane

  int currentGear;

  int roadLength;

  float initialY;
};

}
