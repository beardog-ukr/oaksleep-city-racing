#pragma once

namespace oaksleep_city_racing {

struct MainMenuSceneZOrderValues;
extern const struct MainMenuSceneZOrderValues kMainMenuSceneZO;

struct RoadSceneZOrderValues;
extern const struct RoadSceneZOrderValues kRoadSceneZO;

extern const int kElementsOpacity; //must be 255 in release mode; this value is used
                                   //during physics world deburring
}
