#include "UiWindowsManager.h"
using namespace oaksleep_city_racing;

//#include "ZOrderConstTypes.h"
//#include "ZOrderConstValues.h"

#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

USING_NS_CC;
using namespace std;

static const float kSwapTime = 1.0; // time for nodes to get swapped by swapNodes()

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

UiWindowsManager::UiWindowsManager(const cocos2d::Size& sceneSize,
                                   std::shared_ptr<SixCatsLogger> c6) {
  setLogger(c6);

  firstPosition.x = sceneSize.width + sceneSize.width/2;
  firstPosition.y = sceneSize.height/2;

  secondPosition.x = sceneSize.width/2;
  secondPosition.y = sceneSize.height + sceneSize.height/2;

  normalPosition.x = sceneSize.width/2;
  normalPosition.y = sceneSize.height/2;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

UiWindowsManager::~UiWindowsManager() {
  C6_F1(c6, "here");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void UiWindowsManager::swapBack(cocos2d::Node* current) {
  if (previousNodes.empty() || (current==nullptr)) {
    return;
  }

  MoveTo* removeCurr = MoveTo::create(kSwapTime, firstPosition);
  Sequence* seq = Sequence::create(removeCurr, RemoveSelf::create(), nullptr);
  current->runAction(seq);

  MoveTo* backPrev = MoveTo::create(kSwapTime, normalPosition);
  Node* previousNode = previousNodes.front();
  previousNodes.pop_front();
  previousNode->runAction(backPrev);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

void UiWindowsManager::swapNodes(cocos2d::Node* previous, cocos2d::Node* replacement) {
  if ((previous==nullptr)||(replacement==nullptr)) {
    return;
  }

  replacement->setPosition(firstPosition);

  MoveTo* rmt = MoveTo::create(kSwapTime, normalPosition);
  replacement->runAction(rmt);


  MoveTo* pmt = MoveTo::create(kSwapTime, secondPosition);
  previous->runAction(pmt);

  previousNodes.push_front(previous);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

