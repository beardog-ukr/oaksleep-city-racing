#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"

// using namespace oaksleep_city_racing; does not work

const struct oaksleep_city_racing::MainMenuSceneZOrderValues
oaksleep_city_racing::kMainMenuSceneZO = {
  .background = 10,
  .window = 20,
  .mainLabel = 21,
  .button = 30
};

const struct oaksleep_city_racing::RoadSceneZOrderValues oaksleep_city_racing::kRoadSceneZO = {
  .terrain = 10,
  .road = 11,
  .roadLabel = 12,

  .playerCar = 20,
  .enemyCar = 21,

  .lifesIndicator = 30,
  .gearIndicator = 30
};

//Following value is used during physics world debugging
const int oaksleep_city_racing::kElementsOpacity = 255; //must be 255 in release mode;

const float oaksleep_city_racing::kDesignResolutionWidth = 768.0;
