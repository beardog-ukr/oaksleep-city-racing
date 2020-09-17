#pragma once

#include "cocos2d.h"

#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class StaticElementsKeeper : virtual public SixCatsLoggerLoggable {
public:
  virtual ~StaticElementsKeeper();

  //note this class in not in cocos memory management system, but here is similar create function
  //result is not autoreleased !!!
  static StaticElementsKeeper* create(cocos2d::Scene* roadScene,
                                      std::shared_ptr<SixCatsLogger> c6);

  void setLifesCounter(const int value);// expects values 0..4, lower is more dead
  void setGearIndicator(const int value); // expects 1..4, large is faster

  void setYPosition(const int yPos);

//  void setCamera(cocos2d::Camera* camera);
  void doMove(const std::pair<float, float> moveInfo);
  //const float moveDistance, const float timeInteval);

protected:
  StaticElementsKeeper();
//  virtual ~StaticElementsKeeper();

  bool initSelf(cocos2d::Scene* roadScene);
  bool initGearIndicator(cocos2d::Scene* roadScene);
  bool initLifesWidget(cocos2d::Scene* roadScene);

  void doMoveCamera(const float moveDistance, const float timeInterval);
  void doMoveGearIndicator(const float moveDistance, const float timeInterval);
  void doMoveLifeIndicator(const float moveDistance, const float timeInterval);

  cocos2d::Sprite* lifesSprite;
  cocos2d::Sprite* gearSprite;

  cocos2d::Size windowSize;

  cocos2d::Camera* camera;
};

}
