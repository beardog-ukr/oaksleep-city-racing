#include "RoadNode.h"
using namespace oaksleep_city_racing;

#include "PlayerCarNode.h"
#include "ZOrderConstTypes.h"
#include "ZOrderConstValues.h"

#include "SixCatsLogger.h"
#include "SixCatsLoggerMacro.h"
#include <sstream>

USING_NS_CC;
using namespace std;

static const struct {
  string finishMark;
  string road01;
  string road02;
  string road03;
  string startMark;
  string terrain;
} kSpriteFileNames = {
  .finishMark = "ocr/road/mark_finish",
  .road01 = "ocr/road/tile_00",
  .road02 = "ocr/road/tile_01",
  .road03 = "ocr/road/tile_02",
  // .road03 = "ocr_game/terrain/road_tile_03",
  .startMark = "ocr/road/mark_start",
  .terrain = "ocr/terrain/soil"
};

static const float kDesignResolutionWidth = 768.0;

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

void RoadNode::fillRoadInfo(RoadInfo& roadInfo) {
  roadInfo.roadLength = racingLength;
  roadInfo.startPosition = startPosition;
  roadInfo.enemyFinishPoint = racingLength + windowSize.height;
  roadInfo.screenScaleFactor = windowSize.width / kDesignResolutionWidth;

  if (!PlayerCarNode::fillRoadInfo(roadInfo, windowSize)) {
    C6_D1(c6, "failed to load probe sprite from frame name ");
  }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool RoadNode::initRoad() {
  Sprite* sprite = Sprite::createWithSpriteFrameName(kSpriteFileNames.road01);
  if (sprite == nullptr) {
    C6_C2(c6, "Failed to open ", kSpriteFileNames.road01);
    return false;
  }

  const float screenScaleFactor = windowSize.width / kDesignResolutionWidth;

  Size spriteSize = sprite->getContentSize();
  spriteSize.height = spriteSize.height*screenScaleFactor;
  spriteSize.width = spriteSize.width*screenScaleFactor;

  int xPos = windowSize.width/2;
  int yPos = totalLength; //+ lround(spriteSize.height/2);

  racingLength = 0;

  for (int i = 0; i<150; i++) {
    Sprite* tsp = Sprite::createWithSpriteFrameName(kSpriteFileNames.road01);

    tsp->setAnchorPoint(Vec2(0.5, 0));
    tsp->setPosition(Vec2(xPos, yPos));
    tsp->setOpacity(kElementsOpacity);
    tsp->setScale(screenScaleFactor);

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

  // --- calculate total length
  const int startDistance = totalLength;
  totalLength += racingLength;

  // --- reduce length
  racingLength = racingLength - windowSize.height;
  racingLength += startDistance;

  // --- add finish mark
  sprite = Sprite::createWithSpriteFrameName(kSpriteFileNames.finishMark);
  if (sprite == nullptr) {
    C6_C2(c6, "Failed to open ", kSpriteFileNames.finishMark);
    return false;
  }

  int finishMarkPosition = racingLength + floor(sprite->getContentSize().height/2);
  sprite->setPosition(Vec2(xPos, finishMarkPosition));
  sprite->setOpacity(kElementsOpacity);
  sprite->setScale(screenScaleFactor);

  addChild(sprite, kRoadSceneZO.roadLabel);
  C6_D2(c6, "added finish mark at ", finishMarkPosition);



  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool RoadNode::initRoadStart() {
  totalLength = 0;

//  C6_D2(c6, "Window size height is ", windowSize.height );
  const float screenScaleFactor = windowSize.width / kDesignResolutionWidth;

  int xPos = windowSize.width/2;

  while (totalLength < windowSize.height/2) {
    Sprite* sprite = Sprite::createWithSpriteFrameName(kSpriteFileNames.road01);
    if (sprite == nullptr) {
      C6_C2(c6, "Failed to open ", kSpriteFileNames.road01);
      return false;
    }

    sprite->setScale(screenScaleFactor);
    sprite->setAnchorPoint(Vec2(0.5,0));

    sprite->setPosition(Vec2(xPos, totalLength));
    sprite->setOpacity(kElementsOpacity);

    addChild(sprite, kRoadSceneZO.road);

    const Size spriteSize = sprite->getBoundingBox().size;//  getContentSize();
    C6_T4(c6, "added sprite at ", totalLength, " sprite height is ", spriteSize.height );
    totalLength += spriteSize.height;  // later it will not depend on number of iterations
  }

  Sprite* sprite = Sprite::createWithSpriteFrameName(kSpriteFileNames.startMark);
  if (sprite == nullptr) {
    C6_C2(c6, "Failed to open ", kSpriteFileNames.startMark);
    return false;
  }
  sprite->setScale(screenScaleFactor);

//  startPosition = totalLength + floor(sprite->getContentSize().height/2);
  startPosition = totalLength; //+ floor(sprite->getBoundingBox().size.height/2);
  sprite->setAnchorPoint(Vec2(0.5,0));
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

