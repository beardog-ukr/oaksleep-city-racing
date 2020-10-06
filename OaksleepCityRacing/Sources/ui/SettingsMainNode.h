#pragma once

#include <memory> // shared_ptr
#include <utility> // std::pair

#include "cocos2d.h"

#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class UiWindowsManager;

class SettingsMainNode : public cocos2d::Sprite, virtual public SixCatsLoggerLoggable {
public:
  static SettingsMainNode* create(std::shared_ptr<UiWindowsManager> uwm,
                                  std::shared_ptr<SixCatsLogger> c6);

protected:
  SettingsMainNode();
  virtual ~SettingsMainNode();

  void mcBack(cocos2d::Ref *pSender);

  void mcSound(cocos2d::Ref *pSender);
  void mcDifficulty(cocos2d::Ref *pSender);
  void mcScreenSize(cocos2d::Ref *pSender);

  bool initSelf();
  bool initCaptionLabel();
  bool initMainButtons();
  bool initMenuButtons();

  std::shared_ptr<UiWindowsManager> uiWindowsManager;


};

}
