#pragma once

// #include "cocos2d.h"

namespace oaksleep_city_racing {


class RoadInfo {
public:
  float leftLaneX;
  float rightLaneX;

  float startPosition;

  float turnDistance; // a distance the car travels during lane changing

  float roadLength;
  float enemyFinishPoint;
};

}
