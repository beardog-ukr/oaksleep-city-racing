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
  string terrain;
} kSpriteFileNames = {
  .road01 = "ocr_game/terrain/road_tile_01",
  .road02 = "ocr_game/terrain/road_tile_02",
  .road03 = "ocr_game/terrain/road_tile_03",
  .terrain = "ocr_game/terrain/soil_tile"
};



// static const int kGearMax = 4;
// static const int kLineMax = 2;

// static const int kMoveActionTag = 222;

// static const int kSingleMoveDistance = 400;
// static const float kSingleMoveInterval = 2.0;
// static const int kTurnDistance = 200;

// static const string kRedCarFrameName = "ocr_game/cars/red_car_5";

// static const int kRedCarBodyPointsCount = 12;
// static const Vec2 redCarBodyPoints[kRedCarBodyPointsCount] = {
//   {.x =  14, .y = -78     },
//   {.x = -14, .y = -78     },
//   {.x =  30, .y = -67     },
//   {.x = -29, .y = -67     },
//   {.x =  32, .y =          -43     },
//   {.x = -32, .y =          -43     },
//   {.x =  36, .y =          51      },
//   {.x = -36, .y =          51      },
//   {.x =  28, .y =          69      },
//   {.x = -29, .y =          69      },
//   {.x =  18, .y =          77      },
//   {.x = -16, .y =          77      },
// };

// static const int kPlayerCarCategoryBitmask = 0x01;
// static const int kEnemyCarCategoryBitmask = 0x02;

// const int PlayerCarNode::kTag = 20;

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

RoadNode::RoadNode() {
  lengthTotal = 0;
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

int RoadNode::getLength()  const {
  return lengthTotal;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool RoadNode::initRoad() {
  Sprite* sprite = Sprite::createWithSpriteFrameName(kSpriteFileNames.road01);
  if (sprite == nullptr) {
    C6_C2(c6, "Failed to open ", kSpriteFileNames.road01);
    return 0;
  }

  const Size spriteSize = sprite->getContentSize();
  int xPos = windowSize.width/2;
  int yPos = lround(spriteSize.height/2);

  lengthTotal = 0;

  for (int i = 0; i<100; i++) {
    Sprite* tsp = Sprite::createWithSpriteFrameName(kSpriteFileNames.road01);

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

    yPos += (spriteSize.height -1);
    lengthTotal += spriteSize.height;  // later it will not depend on number of iterations
  }

  return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

bool RoadNode::initSelf(const cocos2d::Size& inWindowSize) {
  windowSize = inWindowSize;

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

  int maxI = (lengthTotal / sprite->getContentSize().height) +1;

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

