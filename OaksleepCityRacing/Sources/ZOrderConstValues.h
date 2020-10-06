#pragma once

namespace oaksleep_city_racing {

struct MainMenuSceneZOrderValues;
extern const struct MainMenuSceneZOrderValues kMainMenuSceneZO;

struct RoadSceneZOrderValues;
extern const struct RoadSceneZOrderValues kRoadSceneZO;

extern const int kElementsOpacity; //must be 255 in release mode; this value is used
                                   //during physics world deburring

extern const float kDesignResolutionWidth; // all images are designed to look best when the screen
                                           //is this wide. If the width of the screen is different
                                           //then scale should be applied to image
}
