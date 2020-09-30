#pragma once

#include <utility> // std::pair

#include "cocos2d.h"

#include "road/DataAdapters.h"

#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class StaticElementsKeeper;

class PlayerCarNode : public cocos2d::Sprite, virtual public SixCatsLoggerLoggable {
public:
  static PlayerCarNode* create(std::shared_ptr<SixCatsLogger> c6);

  void setRoadInfo (const RoadInfo& roadInfo);
  void setStaticElementsKeeper(StaticElementsKeeper* keeper);

  std::pair<float, float> doMove();

  float doMoveToStart(const int windowHeight);


  bool setGearDown();// returns true if gear actually changed
  bool setGearUp();

  bool isTurning();
  bool makeTurnLeft();
  bool makeTurnRight();

  void reactToEnemyContact();

  static const int kTag;
  static const int kTurnDistance;

protected:
  PlayerCarNode();
  virtual ~PlayerCarNode();

  bool initSelf();
  bool initPhysicsBody();

  int currentLaneIndex;
  float lanes[3];// line positions start from right lane
  void doChangeLane();
  bool laneChangeInProgress;

  int currentGear;

  int roadLength;

  float initialY;

  int lifesCounter;
  void doDie();

  int currentMoveSoundId;
  void reevaluateMoveSound(const int soundChange);

  StaticElementsKeeper* staticElementsKeeper;
};

}
