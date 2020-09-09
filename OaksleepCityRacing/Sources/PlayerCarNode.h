#pragma once

#include <utility> // std::pair

#include "cocos2d.h"



#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class StaticElementsKeeper;

class PlayerCarNode : public cocos2d::Sprite, virtual public SixCatsLoggerLoggable {
public:
  static PlayerCarNode* create(std::shared_ptr<SixCatsLogger> c6);

  void setInitialY(const float value);
  void setLanes(const float leftLaneX, const float rightLaneX);
  void setRoadLength(const int roadLength);
  void setStaticElementsKeeper(StaticElementsKeeper* keeper);

  std::pair<float, float> doMove();

  bool setGearDown();// returns true if gear actually changed
  bool setGearUp();

//  std::pair<float, float>  doTurnRight();
//  std::pair<float, float>  doTurnLeft();
  bool makeTurnLeft();
  bool makeTurnRight();

  // void setGamePosition(const int gameX, const int gameY);
  // void doAttackTo(const int gameX, const int gameY);
  // void doChangePositionTo(const int gameX, const int gameY);

  // int getValue() const;

  // static void unloadAnimations();

protected:
  PlayerCarNode();
  virtual ~PlayerCarNode();

  bool initSelf();

  int currentLaneIndex;
  float lanes[3];// line positions start from right lane
  void doChangeLane();

  int currentGear;

  int roadLength;

  float initialY;

  StaticElementsKeeper* staticElementsKeeper;

//  cocos2d::Camera* camera;
//  float cameraCarPosDifference;

//  void doMoveSelf();//const float newX, const float newY);
//  void doMoveStaticElements(const int moveDistance);

  // float iterationDuration;

  // cocos2d::Vec2 calculatePosition(const int x, const int y) const;
};

}
