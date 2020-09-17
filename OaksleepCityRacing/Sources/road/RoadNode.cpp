#include "RoadNode.h"
using namespace oaksleep_city_racing;

#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"

#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

USING_NS_CC;
using namespace std;

static const struct {
  string road01;
  string road02;
  string road03;
  string startMark;
  string terrain;
} kSpriteFileNames = {
  .road01 = "ocr_game/terrain/road_tile_01",
  .road02 = "ocr_game/terrain/road_tile_02",
  .road03 = "ocr_game/terrain/road_tile_03",
  .startMark = "ocr_game/terrain/mark_start",
  .terrain = "ocr_game/terrain/soil_tile"
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

RoadNode::RoadNode() {
  racingLength = 0;
  totalLength = 0;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

RoadNode::~RoadNode() {
  C6_F1(c6, "here");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

RoadNode* RoadNode::create(const cocos2d::Size& inWindowSize, shared_ptr<SixCatsLogger> c6) {

  RoadNode *pRet = new(std::nothrow) RoadNode();
  if (pRet ==nullptr) {
    return nullptr;
  }

  pRet->setLogger(c6);

  if (!pRet->initSelf(inWindowSize)) {
    delete pRet;
    pRet = nullptr;
    return nullptr;
  }

  pRet->autorelease();
  return pRet;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

// int RoadNode::getLength()  const {
//   return racingLength;
// }

void RoadNode::fillRoadInfo(RoadInfo& roadInfo) {
  roadInfo.roadLength = racingLength;
  roadInfo.startPosition = startPosition;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool RoadNode::initRoad() {
  Sprite* sprite = Sprite::createWithSpriteFrameName(kSpriteFileNames.road01);
  if (sprite == nullptr) {
    C6_C2(c6, "Failed to open ", kSpriteFileNames.road01);
    return false;
  }

  const Size spriteSize = sprite->getContentSize();
  int xPos = windowSize.width/2;
  int yPos = totalLength; //+ lround(spriteSize.height/2);

  racingLength = 0;

  for (int i = 0; i<100; i++) {
    Sprite* tsp = Sprite::createWithSpriteFrameName(kSpriteFileNames.road01);

    tsp->setAnchorPoint(Vec2(0.5, 0));
    tsp->setPosition(Vec2(xPos, yPos));
    tsp->setOpacity(kElementsOpacity);

    addChild(tsp, kRoadSceneZO.road);

    if ((i%10==0)) {
      std::ostringstream oss;
      oss << "L:" << i;

      Label* label = Label::createWithTTF(oss.str(), "fonts/Marker Felt.ttf", 24);
      if (label == nullptr) {
        C6_C1(c6, "Failed to open 'fonts/Marker Felt.ttf'");
      }
      else {
        // position the label on the center of the screen
        label->setPosition(Vec2(windowSize.width/2 - windowSize.width/8, yPos));

        // add the label as a child to this layer
        this->addChild(label, kRoadSceneZO.roadLabel);
      }
    }

    yPos += (spriteSize.height );//-1);
    racingLength += spriteSize.height;  // later it will not depend on number of iterations
  }

  totalLength += racingLength;

  // --- reduce length
  racingLength = racingLength - 640;

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool RoadNode::initRoadStart() {
  totalLength = 0;

  C6_D2(c6, "Window size height is ", windowSize.height );

//  const Size spriteSize = sprite->getContentSize();
  int xPos = windowSize.width/2;
//  int yPos = 0;// lround(spriteSize.height/2);

  while (totalLength < windowSize.height/2) {
    Sprite* sprite = Sprite::createWithSpriteFrameName(kSpriteFileNames.road01);
    if (sprite == nullptr) {
      C6_C2(c6, "Failed to open ", kSpriteFileNames.road01);
      return false;
    }

    sprite->setAnchorPoint(Vec2(0.5,0));

    const Size spriteSize = sprite->getContentSize();

//    int yPos = totalLength + floor(spriteSize.height/2) -1;

    sprite->setPosition(Vec2(xPos, totalLength));
    sprite->setOpacity(kElementsOpacity);

    addChild(sprite, kRoadSceneZO.road);
    C6_D4(c6, "added sprite at ", totalLength, " sprite height is ", spriteSize.height );


//    yPos += (spriteSize.height -1);
    totalLength += spriteSize.height;  // later it will not depend on number of iterations
  }

  Sprite* sprite = Sprite::createWithSpriteFrameName(kSpriteFileNames.startMark);
  if (sprite == nullptr) {
    C6_C2(c6, "Failed to open ", kSpriteFileNames.startMark);
    return false;
  }

  startPosition = totalLength + floor(sprite->getContentSize().height/2);
  sprite->setPosition(Vec2(xPos, startPosition));
  sprite->setOpacity(kElementsOpacity);

  addChild(sprite, kRoadSceneZO.roadLabel);
  C6_D2(c6, "added start mark at ", startPosition);

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool RoadNode::initSelf(const cocos2d::Size& inWindowSize) {
  windowSize = inWindowSize;

  if (!initRoadStart()) {
    return false;
  }

  if (!initRoad()) {
    return false;
  }

  if (!initTerrain()) {
    return false;
  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool RoadNode::initTerrain() {
//  const Size currentWindowSize = getContentSize();
  Sprite* sprite = Sprite::createWithSpriteFrameName(kSpriteFileNames.terrain);
  if (sprite == nullptr) {
    C6_C2(c6, "Failed to open ", kSpriteFileNames.terrain);
    return false;
  }

  const Size spriteSize = sprite->getContentSize();
//  int xPos = windowSize.width - spriteSize.width/2;
  int yPos = 0;

  int maxI = (totalLength / sprite->getContentSize().height) +1;

  for (int i = 0; i<maxI; i++) {
    Sprite* tsp = Sprite::createWithSpriteFrameName(kSpriteFileNames.terrain);
    tsp->setPosition(Vec2(0, yPos));
    tsp->setOpacity(kElementsOpacity);
    addChild(tsp, kRoadSceneZO.terrain);

    tsp = Sprite::createWithSpriteFrameName(kSpriteFileNames.terrain);
    tsp->setPosition(Vec2(windowSize.width, yPos));
    tsp->setOpacity(kElementsOpacity);
    addChild(tsp, kRoadSceneZO.terrain);

    yPos += (spriteSize.height -1);
  }

  return true;
}


// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

