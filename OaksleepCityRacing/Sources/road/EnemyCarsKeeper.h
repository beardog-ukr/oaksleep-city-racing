#pragma once

#include <list> //

#include "cocos2d.h"

#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class EnemyCarNode;

class EnemyCarsKeeper : virtual public SixCatsLoggerLoggable {
public:
  static EnemyCarsKeeper* create(std::shared_ptr<SixCatsLogger> c6);
  EnemyCarsKeeper();
  virtual ~EnemyCarsKeeper();//TODO make private

//  void setRoadLength(const int roadLength);
  bool generateCars(const int roadLength, cocos2d::Node* carsParent);

protected:


  bool initSelf();

  std::list<EnemyCarNode*> cars;

  int roadLength;
};

}
