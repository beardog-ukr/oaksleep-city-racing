#pragma once

namespace oaksleep_city_racing {

struct MainMenuSceneZOrderValues {
  int background;
  int window;
  int mainLabel;
  int button;
};

struct RoadSceneZOrderValues {
  int terrain;
  int road;
  int roadLabel;

  int playerCar;
  int enemyCar;

  int lifesIndicator;
  int gearIndicator;
};

}
