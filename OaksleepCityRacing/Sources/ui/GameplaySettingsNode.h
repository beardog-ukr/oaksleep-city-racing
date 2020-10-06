#pragma once

#include <memory> // shared_ptr
#include <utility> // std::pair

#include "cocos2d.h"

#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class UiWindowsManager;

class GameplaySettingsNode : public cocos2d::Sprite, virtual public SixCatsLoggerLoggable {
public:
  static GameplaySettingsNode* create(std::shared_ptr<UiWindowsManager> uwm,
                                      std::shared_ptr<SixCatsLogger> c6);

protected:
  GameplaySettingsNode();
  virtual ~GameplaySettingsNode();

  void mcBack(cocos2d::Ref *pSender);

  void changeCarProbValue(cocos2d::Ref *pSender, const bool doIncrement);
  void changeRightLaneProbValue(cocos2d::Ref *pSender, const bool doIncrement);

  bool initSelf();
  bool initCaptionLabel();
  bool initMenuButtons();
  bool initCarProbPanel();
  bool initRightLaneProbPanel();

  cocos2d::Label* rightLaneProbLabel;
  int rightLaneProb;

  cocos2d::Label* carProbLabel;
  int carProb;

  std::shared_ptr<UiWindowsManager> uiWindowsManager;


};

}
