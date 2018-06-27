//
//  ZXObjectFactory.h
//  PaperPlane
//
//  Created by Zolkin on 10/13/13.
//
//

#ifndef __PaperPlane__ZXObjectFactory__
#define __PaperPlane__ZXObjectFactory__

#include "cocos2d.h"
#include "GeometryExt.h"

class ZXCollideObject;
class ZXSphereCollider;

enum ColideObjectType
{
    kColliderType_Ground = 1,
    kColliderType_Tree,
    kColliderType_CloudEmpty,
    kColliderType_CloudRain,
    kColliderType_CloudThunder,
    kColliderType_Coin,
    kColliderType_CoinMagnet,
    kColliderType_BonusMagnet,
    kColliderType_Tornado,
    kColliderType_UpgradeUmbrella,
    kColliderType_UpgradeShield,
    kColliderType_UpgradeDrop,
    kColliderType_UpgradeMagnet,
    kColliderType_UpgradeElixir,
    kColliderType_UpgradeEnd,
};

enum ZXTrashObjectType
{
    kZXTrashObjectType_Bee = 0,
    kZXTrashObjectType_Dandelion = 1,
    kZXTrashObjectType_Bug = 2,
};

enum ZXBonusObjectType
{
    kZXBonusObjectType_Magnet = 0,
};

enum AdditionalBehaviourType
{
    kAnimation_pulse = 1,
    kAnimation_up_down,
    kAnimation_up_down_random,
    kAnimation_up_down_random2,
};

struct ZXObjectData
{
    std::string filePath;
    std::string animationPath;
    std::string animationName;
    std::vector<AdditionalBehaviourType> objBehavArray;
    float colliderRadius;
    int type;
};

class ZXObjectFactory
{
public:
    static ZXObjectFactory* Instanse();

    cocos2d::Animate* createAnimation(const char* fileName, int frameCount, float delay, bool restoreToOriginalFrame, const char* format = "png");
    cocos2d::Animate* createAnimation(const char* fileName, const char* animationName);
    cocos2d::Animate* createAnimation(const char* fileName, const char* animationName, int framesCount, float delay);
    void createLandscapeCollider(ZXCollideObject* sprite, cocos2d::PointArray* points);
    ZXSphereCollider* createSphereCollider(ZXCollideObject* sprite, Circle2d circle);
    ZXCollideObject* createTree(const char* spritePath, cocos2d::Point pos);
    ZXCollideObject* createGround(const char *landSpritePath, cocos2d::PointArray* points);
    ZXCollideObject* createCloud(int type);
    ZXCollideObject* createCoin(bool withCollider = true);
    ZXCollideObject* createTrashObject(int type);
    ZXCollideObject* createBonusObject(int type);
    ZXCollideObject* createRandomUpgrade();
    
private:
    void init();
    cocos2d::Action* createAdditionalBehaviour(AdditionalBehaviourType type);
    
    std::vector<ZXObjectData> mClouds;
    std::vector<ZXObjectData> mTrashObjects;
    std::vector<ZXObjectData> mBonusObjects;
    static ZXObjectFactory* mInstance;
};

#endif /* defined(__PaperPlane__ZXObjectFactory__) */
