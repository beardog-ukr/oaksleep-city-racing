#pragma once

#include <utility> // std::pair

#include "cocos2d.h"



#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class PlayerCarNode : public cocos2d::Sprite, virtual public SixCatsLoggerLoggable {
public:
  static PlayerCarNode* create(std::shared_ptr<SixCatsLogger> c6);

  void setInitialPos(const cocos2d::Vec2 carPos);
  void setLines(const float leftLineX, const float rightLineX);
  void setRoadLength(const int roadLength);

  std::pair<float, float> doMove();

  bool setGearDown();// returns true if gear actually changed
  bool setGearUp();

  // void setGamePosition(const int gameX, const int gameY);
  // void doAttackTo(const int gameX, const int gameY);
  // void doChangePositionTo(const int gameX, const int gameY);

  // int getValue() const;

  // static void unloadAnimations();

protected:
  PlayerCarNode();
  virtual ~PlayerCarNode();

  bool initSelf();

  int currentLineIndex;
  float linePositions[3];

  int currentGear;

  int roadLength;

  cocos2d::Camera* camera;
  float cameraCarPosDifference;

//  void doMoveSelf();//const float newX, const float newY);
//  void doMoveStaticElements(const int moveDistance);

  // float iterationDuration;

  // cocos2d::Vec2 calculatePosition(const int x, const int y) const;
};

}
