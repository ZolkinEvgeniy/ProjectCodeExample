//
//  ZXObjectFactory.cpp
//  PaperPlane
//
//  Created by Zolkin on 10/13/13.
//
//

#include "ZXObjectFactory.h"
#include "ZXCollideObject.h"
#include "ZXResources.h"
#include "GameSettings.h"
#include "ZXPoligonCollider.h"
#include "ZXSphereCollider.h"
#include "ZXCoinCollider.h"
#include "ZXParticleRain.h"
#include "ZXTreeObject.h"
#include "ZXCoinObject.h"
#include "ZXGameManager.h"

USING_NS_CC;

ZXObjectFactory* ZXObjectFactory::mInstance = NULL;

ZXObjectFactory* ZXObjectFactory::Instanse()
{
    if (!mInstance)
    {
        mInstance = new ZXObjectFactory();
        mInstance->init();
    }
    return mInstance;
}

void ZXObjectFactory::init()
{
    ZXObjectData cloudData1;
    cloudData1.filePath = "cloud_1.png";
    cloudData1.type = kColliderType_CloudEmpty;
    cloudData1.objBehavArray.push_back(kAnimation_up_down);
    cloudData1.colliderRadius = 80;
    mClouds.push_back(cloudData1);
    
    ZXObjectData cloudData2;
    cloudData2.filePath = "cloud_rain.png";
    cloudData2.type = kColliderType_CloudRain;
    cloudData2.objBehavArray.push_back(kAnimation_pulse);
    mClouds.push_back(cloudData2);
    
    ZXObjectData cloudData3;
    cloudData3.filePath = "cloud_thunder_1.png";
    cloudData3.animationPath = "animations/clouds_anim.plist";
    cloudData3.animationName = "cloud_thunder";
    cloudData3.type = kColliderType_CloudThunder;
    cloudData3.colliderRadius = 80;
    mClouds.push_back(cloudData3);
    
    ZXObjectData cloudData4;
    cloudData4.filePath = "cloud_1.png";
    cloudData4.type = kColliderType_CloudEmpty;
    cloudData4.objBehavArray.push_back(kAnimation_up_down_random2);
    cloudData3.colliderRadius = 80;
    mClouds.push_back(cloudData4);
    
    ZXObjectData tornadoData1;
    tornadoData1.filePath = "tornado_1.png";
    tornadoData1.animationPath = "animations/tornado_anim.plist";
    tornadoData1.animationName = "tornado";
    tornadoData1.type = kColliderType_Tornado;
    tornadoData1.colliderRadius = 100;
    mClouds.push_back(tornadoData1);
    
    ZXObjectData trashObjectData1;
    trashObjectData1.filePath = "bee_1.png";
    trashObjectData1.animationPath = "animations/sun_anim.plist";
    trashObjectData1.animationName = "bee";
    trashObjectData1.type = kZXTrashObjectType_Bee;
    trashObjectData1.objBehavArray.push_back(kAnimation_up_down_random);
    mTrashObjects.push_back(trashObjectData1);
    
    ZXObjectData trashObjectData2;
    trashObjectData2.filePath = "dandelion.png";
    trashObjectData2.type = kZXTrashObjectType_Dandelion;
    trashObjectData2.objBehavArray.push_back(kAnimation_up_down_random);
    mTrashObjects.push_back(trashObjectData2);
    
    ZXObjectData trashObjectData3;
    trashObjectData3.filePath = "bug_1.png";
    trashObjectData3.animationPath = "animations/sun_anim.plist";
    trashObjectData3.animationName = "bug";
    trashObjectData3.type = kZXTrashObjectType_Bug;
    trashObjectData3.objBehavArray.push_back(kAnimation_up_down_random);
    mTrashObjects.push_back(trashObjectData3);
    
    ZXObjectData bonusObjectData1;
    bonusObjectData1.filePath = "magnit_1.png";
    bonusObjectData1.animationPath = "animations/magnit_anim.plist";
    bonusObjectData1.animationName = "magnit";
    bonusObjectData1.type = kColliderType_BonusMagnet;
    mBonusObjects.push_back(bonusObjectData1);
}

cocos2d::Animate* ZXObjectFactory::createAnimation(const char* fileName, const char* animationName)
{
    AnimationCache *cache = AnimationCache::getInstance();
    cache->addAnimationsWithFile(fileName);
    Animation* animation = cache->getAnimation(animationName);  // I apologize for this method name, it should be getAnimationByName(..) in future versions
    Animate* animate = Animate::create(animation);  // Don't confused between CCAnimation and CCAnimate :)
    return animate;
}

cocos2d::Animate* ZXObjectFactory::createAnimation(const char* fileName, const char* animationName, int framesCount, float delay)
{
    SpriteFrameCache* spriteCache = SpriteFrameCache::getInstance();
    spriteCache->addSpriteFramesWithFile(String::createWithFormat("%s.plist", fileName)->getCString());
    
    Array* frames = Array::create();
    for (int i = 1; i <= framesCount; i++)
    {
        SpriteFrame* frame = spriteCache->getSpriteFrameByName(String::createWithFormat("%s_%d.png", animationName, i)->getCString());
        frames->addObject(frame);
    }
    Animation* animation = Animation::createWithSpriteFrames(frames, delay);
    Animate* animate = Animate::create(animation);
    return animate;
}

cocos2d::Animate* ZXObjectFactory::createAnimation(const char* fileName, int framesCount, float delay, bool restoreToOriginalFrame, const char* format)
{
    Animation *animation = Animation::create();
    
    for (int i = 1; i <= framesCount; i++)
    {
        String* path = String::createWithFormat("%s_%d.%s", fileName, i, format);
        animation->addSpriteFrameWithFile(path->getCString());
    }
    
    animation->setDelayPerUnit(delay);
    animation->setRestoreOriginalFrame(restoreToOriginalFrame); // Return to the 1st frame after the last frame is played.
    
    Animate *action = Animate::create(animation);
    return action;
}

cocos2d::Action* ZXObjectFactory::createAdditionalBehaviour(AdditionalBehaviourType BehType)
{
    RepeatForever * repSeq= NULL;
    switch (BehType) {
        case kAnimation_pulse :
        {
            ActionInterval *zoomInAction = ScaleTo::create(2, 1.2f);
            ActionInterval *zoomOutAction = ScaleTo::create(2, 1.f);
            Sequence * seqZoom = Sequence::create(zoomInAction, zoomOutAction, NULL);
            repSeq = RepeatForever::create(seqZoom );
            break;
        }
        case kAnimation_up_down:
        {
            ActionInterval *moveUp = MoveBy::create(2, AdaptedPoint(0,20));
            ActionInterval *moveDown = MoveBy::create(2, AdaptedPoint(0,-20));
            Sequence * seqMove = Sequence::create(moveUp,moveDown, NULL);
            repSeq = RepeatForever::create(seqMove );
            break;
        }
        case kAnimation_up_down_random:
        {
            float rnd = arc4random_uniform(100);
            ActionInterval *moveUp = MoveBy::create(2, AdaptedPoint(0, rnd));
            ActionInterval *moveDown = MoveBy::create(2, AdaptedPoint(0,-rnd));
            Sequence * seqMove = Sequence::create(moveUp,moveDown, NULL);
            repSeq = RepeatForever::create(seqMove );
            break;
        }
        case kAnimation_up_down_random2:
        {
            int min = 100;
            int max = 300;
            float rnd = arc4random_uniform(max - min) + min;
            ActionInterval *moveUp = MoveBy::create(3 * rnd / max, AdaptedPoint(0, rnd));
            ActionInterval *moveDown = MoveBy::create(3 * rnd / max, AdaptedPoint(0,-rnd));
            Sequence * seqMove = Sequence::create(moveUp,moveDown, NULL);
            repSeq = RepeatForever::create(seqMove );
            break;
        }
         default:
            break;
    }
    return repSeq;
}

ZXCollideObject* ZXObjectFactory::createCloud(int type)
{
    CCASSERT(type < mClouds.size(), "Cloud type not exists");
    ZXObjectData cloudData = mClouds[type];
    
    ZXCollideObject* objectSprite = ZXCollideObject::createWithSpriteFrameName(cloudData.filePath.c_str());

    if (!cloudData.animationPath.empty())
    {
        Animate* animate = createAnimation(cloudData.animationPath.c_str(), cloudData.animationName.c_str());
        Action* action = RepeatForever::create(animate);
        objectSprite->addAnimation(action);
    }
    for(auto itr = cloudData.objBehavArray.begin();itr != cloudData.objBehavArray.end(); itr++)
    {
        objectSprite->addAnimation(createAdditionalBehaviour(*itr));
    }
    objectSprite->setType(cloudData.type);
    
    if(cloudData.type == kColliderType_CloudRain)
    {
        ParticleSystem* emitter = ZXParticleRain::create();
        objectSprite->addChild(emitter, 10);
        emitter->setPosition(Point(objectSprite->getContentSize().width * 0.5, objectSprite->getContentSize().height * 0.2));
        
        emitter->setTexture( TextureCache::getInstance()->addImage(particle_drop) );
        emitter->setPosVar(Point(objectSprite->getContentSize().width * 0.3, 0));
        
        Size contentSize = objectSprite->getContentSize();
        PointArray* points1 = PointArray::create(10);
        points1->addControlPoint(Point(contentSize.width * 0.7, contentSize.height * 0.2));
        points1->addControlPoint(Point(contentSize.width * 0.1, contentSize.height * 0.2));
        points1->addControlPoint(Point(-contentSize.width * 0.1, -contentSize.height * 0.5));
        points1->addControlPoint(Point(contentSize.width - contentSize.width * 0.5, -contentSize.height * 0.5));
        
        createLandscapeCollider(objectSprite, points1);
    }
    else
    {
        createSphereCollider(objectSprite, Circle2d( Point(objectSprite->getContentSize().width * 0.5, objectSprite->getContentSize().height * 0.5), DeviceScaling.width * cloudData.colliderRadius));
    }
    return objectSprite;
}

ZXCollideObject* ZXObjectFactory::createCoin(bool withCollider)
{
    Animate* anim = createAnimation("animations/sun_anim.plist", "sun");

    ZXCoinObject* object = ZXCoinObject::createWithSpriteFrameName("sun_1.png");
    object->setAnchorPoint(Point(0.5,0.5));
    if (withCollider)
    {
        createSphereCollider(object, Circle2d( Point(object->getContentSize().width * 0.5, object->getContentSize().height * 0.5), DeviceScaling.width * 30));

        // Creating magnet collider
        ZXCollideObject* magnetObject = ZXCollideObject::create();
        magnetObject->setType(kColliderType_CoinMagnet);
        object->addChild(magnetObject);
        ZXCoinCollider* magnetCollider = ZXCoinCollider::create();
        magnetCollider->setRadius(ZXGameManager::Instance()->GameParams().magnetRadius * VISIBLE_SIZE.height);
        magnetCollider->setPosition(Point(object->getContentSize().width * 0.5, object->getContentSize().height * 0.5));
        magnetObject->setCollider(magnetCollider);
        object->setMagnetCollider(magnetCollider);
    }
    object->setType(kColliderType_Coin);

    object->addAnimation(RepeatForever::create(anim));
    
    return object;
}

ZXCollideObject* ZXObjectFactory::createRandomUpgrade()
{
    static std::map<int, std::string> upgradeNames;
    if (upgradeNames.empty())
    {
        upgradeNames.insert(std::make_pair(kColliderType_UpgradeUmbrella, upgrade_umbrella_image));
        upgradeNames.insert(std::make_pair(kColliderType_UpgradeShield, upgrade_shield_image));
        upgradeNames.insert(std::make_pair(kColliderType_UpgradeDrop, upgrade_drop_image));
    }
    int rnd = arc4random_uniform(upgradeNames.size());
    int cnt = 0;
    auto itr = upgradeNames.begin();
    while (itr != upgradeNames.end())
    {
        if (cnt == rnd) {
            break;
        }
        ++cnt; ++itr;
    }
    CCASSERT(itr != upgradeNames.end(), "Iterator = upgradeNames.end()");
    ZXCollideObject* objectSprite = ZXCollideObject::create(itr->second.c_str());
    objectSprite->setType(itr->first);
    return objectSprite;
}

ZXCollideObject* ZXObjectFactory::createTrashObject(int type)
{
    CCASSERT(type < mTrashObjects.size(), "Trash type not exists");
    ZXObjectData objectData = mTrashObjects[type];
    
    ZXCollideObject* objectSprite = ZXCollideObject::createWithSpriteFrameName(objectData.filePath.c_str());
    
    if (!objectData.animationPath.empty())
    {
        Animate* animate = createAnimation(objectData.animationPath.c_str(), objectData.animationName.c_str());
        Action* action = RepeatForever::create(animate);
        objectSprite->addAnimation(action);
    }
    for(auto itr = objectData.objBehavArray.begin();itr != objectData.objBehavArray.end(); itr++)
    {
        objectSprite->addAnimation(createAdditionalBehaviour(*itr));
    }
    objectSprite->setType(objectData.type);
    return objectSprite;
}

ZXCollideObject* ZXObjectFactory::createBonusObject(int type)
{
    CCASSERT(type < mBonusObjects.size(), "Bonus type not exists");
    ZXObjectData objectData = mBonusObjects[type];
    
    Animate* animate = createAnimation(objectData.animationPath.c_str(), objectData.animationName.c_str());

    ZXCollideObject* objectSprite = ZXCollideObject::createWithSpriteFrameName(objectData.filePath.c_str());
    
    Action* action = RepeatForever::create(animate);
    objectSprite->addAnimation(action);

    createSphereCollider(objectSprite, Circle2d( Point(objectSprite->getContentSize().width * 0.5, objectSprite->getContentSize().height * 0.5), DeviceScaling.width * 50));

    for(auto itr = objectData.objBehavArray.begin();itr != objectData.objBehavArray.end(); itr++)
    {
        objectSprite->addAnimation(createAdditionalBehaviour(*itr));
    }
    objectSprite->setType(objectData.type);
    return objectSprite;
}

ZXCollideObject* ZXObjectFactory::createTree(const char *spritePath, cocos2d::Point pos)
{
    ZXTreeObject* objectSprite = ZXTreeObject::createWithSpriteFrameName(spritePath);
    createSphereCollider(objectSprite, Circle2d( Point(objectSprite->getContentSize().width * 0.5, objectSprite->getContentSize().height * 0.5), DeviceScaling.width * 80));
    objectSprite->setType(kColliderType_Tree);
    objectSprite->setPosition(pos);
    
    Sprite* obj = Sprite::createWithSpriteFrameName("tree_leaf_0.png");
    Action* anim = createAnimation("animations/tree_leafs_anim.plist", "tree_leafs");
    obj->setAnchorPoint(Point(0, -0.2));
    objectSprite->addCollideObect(obj, anim);
    objectSprite->addChild(obj);

    return objectSprite;
}

ZXCollideObject* ZXObjectFactory::createGround(const char *spritePath, PointArray *points)
{
    ZXCollideObject* landSprite = ZXCollideObject::createWithSpriteFrameName(spritePath);
    landSprite->setAnchorPoint(Point::ZERO);
    createLandscapeCollider(landSprite, points);
    landSprite->setType(kColliderType_Ground);
    return landSprite;
}

void ZXObjectFactory::createLandscapeCollider(ZXCollideObject *sprite, cocos2d::PointArray *points)
{
    ZXPoligonCollider* collider = ZXPoligonCollider::create();
    for (int i = 0; i < points->count()-1; i++)
    {
        collider->addSegment(Line2d(points->getControlPointAtIndex(i), points->getControlPointAtIndex(i+1)));
    }
    sprite->setCollider(collider);
}

ZXSphereCollider* ZXObjectFactory::createSphereCollider(ZXCollideObject *sprite, Circle2d circle)
{
    ZXSphereCollider* sphereCollider = ZXSphereCollider::create();
    sphereCollider->setRadius(circle.radius);
    sphereCollider->setPosition(circle.position);
    sprite->setCollider(sphereCollider);
    return sphereCollider;
}

