#pragma once

#include <memory> // shared_ptr
#include <utility> // std::pair

#include "cocos2d.h"

#include "SixCatsLoggerLoggable.h"

namespace oaksleep_city_racing {

class UiWindowsManager;

class ScreenSizeNode : public cocos2d::Sprite, virtual public SixCatsLoggerLoggable {
public:
  static ScreenSizeNode* create(std::shared_ptr<UiWindowsManager> uwm,
                                std::shared_ptr<SixCatsLogger> c6);

protected:
  ScreenSizeNode();
  virtual ~ScreenSizeNode();

  void mcBack(cocos2d::Ref *pSender);

  void mcApply(cocos2d::Ref *pSender);
  void changeValue(cocos2d::Ref *pSender, const bool doIncrement);

  bool initSelf();
  bool initCaptionLabel();
  bool initApplyLabel();
  bool initMenuButtons();
  bool initValuePanel();

  cocos2d::Label* valueLabel;
  int selectedSettingIdx;

  std::shared_ptr<UiWindowsManager> uiWindowsManager;


};

}
