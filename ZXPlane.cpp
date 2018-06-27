//
//  Plane.cpp
//  PaperPlane
//
//  Created by Zolkin on 9/26/13.
//
//
#define DURATION_FOR_MOVE_BACK 1

#include "ZXPlane.h"
#include "GameSettings.h"
#include "ZXGameManager.h"
#include "ZXObjectFactory.h"
#include "ZXResources.h"
#include "ZXCollideObject.h"
#include "ZXParticleSmoke.h"

USING_NS_CC;

static const float kMaxPlanePosX = 0.5f;
static const float kSpeedPlaneX = 0.5f;
static const float kMaxDistanceForMoveback = 100.0f;
static const int   kBurningPeriod = 12;
static const float kSpeedXBack = -4.0f;
static const float kSpeedXBackMax = -10.0f;
static const float kSpeedXBackPlus = -7.0f;

static const float kPlaneStateTime_Shield = 5.0f;
//static const float kPlaneStateTime_Burn = 8.0f;
static const float kPlaneStateTime_Wet = 8.0f;

enum PlaneChildTag
{
    kPlaneChildTag_Smoke = 100,
};

enum PlaneActionTag
{
    kPlaneActionTag_Burn = 1,
    kPlaneActionTag_Wet,
    kPlaneActionTag_Shield,
    kPlaneActionTag_MoveBack,
    kPlaneActionTag_Forward,
    kPlaneActionTag_UpDown,
};

ZXPlane* ZXPlane::create()
{
    ZXPlane *pobSprite = new ZXPlane();
    if (pobSprite && pobSprite->init())
    {
        pobSprite->autorelease();
        return pobSprite;
    }
    CC_SAFE_DELETE(pobSprite);
    return NULL;
}

ZXPlane::~ZXPlane()
{
    CC_SAFE_RELEASE(mAnimations);
    //CC_SAFE_RELEASE(mDefendedObjects);
}

bool ZXPlane::init()
{
    if(!Node::init())
    {
        return false;
    }
    
    loadParameters();
    mLowerLimit = AdaptedPoint(0, 125).y;
    mUpperLimit = AdaptedPoint(0, 680).y;
    mRadius = DeviceScaling.width * 30;
    mSpeedX = kSpeedPlaneX * DeviceScaling.width;
    mCanSimulate = true;
    mNeedRotation = false;
    mDeformationState = PlaneAnimation_100;
    
    //mDefendedObjects = Array::create();
    
    mPlaneSprite = Sprite::create("images/game_layer/plane/plane_1.png");
    mPlaneSprite->setAnchorPoint(Point::ZERO);
    addChild(mPlaneSprite, -1);
    setContentSize(mPlaneSprite->getContentSize());
//    setTexture(texture);
//    Rect rect = Rect::ZERO;
//    rect.size = texture->getContentSize();
//    setTextureRect(rect);
    
    //setAnchorPoint(Point(0.5, 0.5));
    
    mShieldSprite = Sprite::create(upgrade_shield_plane_image);
    mShieldSprite->setAnchorPoint(Point(0.5, 0.5));
    mShieldSprite->setPosition(Point(getContentSize().width * 0.5, getContentSize().height * 0.5));
    addChild(mShieldSprite);
    mShieldSprite->setVisible(false);
    
    createAnimations();

    //changeState(kPlaneState_Shield);
    changeState(kPlaneState_Forward);

    scheduleUpdate();
    
    scheduleOnce(schedule_selector(ZXPlane::nonGravitationFinished), 2); // Activate gravitation after delay
    
    return true;
}

void ZXPlane::loadParameters()
{
    ZXGameParameters gameParams = ZXGameManager::Instance()->GameParams();

    mMass = gameParams.planeMass;
    mMassAdditionalMax = gameParams.additionalMass;
    mGravitation = Point(0,0);// -gameParams.gravitation);
    mAirFrictionConstant = 0.02;
    mLinearDamping = gameParams.damping;
}

void ZXPlane::nonGravitationFinished(float dt)
{
    ZXGameParameters gameParams = ZXGameManager::Instance()->GameParams();
    
    mGravitation = Point(0, -gameParams.gravitation);
}

void ZXPlane::createAnimations()
{
    mAnimations = Dictionary::create();
    mAnimations->retain();
    
    AnimationCache *cache = AnimationCache::getInstance();
    cache->addAnimationsWithFile("animations/plane_anim.plist");
    Animation* animation = cache->getAnimation("plane_50");
    Animate* animate = Animate::create(animation);
    Action* action = RepeatForever::create(animate);
    action->setTag(kPlaneActionTag_Forward);
    mAnimations->setObject(action, PlaneAnimation_50);

    animation = cache->getAnimation("plane_75");
    animate = Animate::create(animation);
    action = RepeatForever::create(animate);
    action->setTag(kPlaneActionTag_Forward);
    mAnimations->setObject(action, PlaneAnimation_75);

    animation = cache->getAnimation("plane_100");
    animate = Animate::create(animation);
    action = RepeatForever::create(animate);
    action->setTag(kPlaneActionTag_Forward);
    mAnimations->setObject(action, PlaneAnimation_100);
    
    animation = cache->getAnimation("plane_down_50");
    animate = Animate::create(animation);
    action = RepeatForever::create(animate);
    action->setTag(kPlaneActionTag_Forward);
    mAnimations->setObject(action, PlaneAnimation_down_50);
    
    animation = cache->getAnimation("plane_down_75");
    animate = Animate::create(animation);
    action = RepeatForever::create(animate);
    action->setTag(kPlaneActionTag_Forward);
    mAnimations->setObject(action, PlaneAnimation_down_75);
    
    animation = cache->getAnimation("plane_down_100");
    animate = Animate::create(animation);
    action = RepeatForever::create(animate);
    action->setTag(kPlaneActionTag_Forward);
    mAnimations->setObject(action, PlaneAnimation_down_100);
    
    animation = cache->getAnimation("plane_up_50");
    animate = Animate::create(animation);
    action = RepeatForever::create(animate);
    action->setTag(kPlaneActionTag_Forward);
    mAnimations->setObject(action, PlaneAnimation_up_50);
    
    animation = cache->getAnimation("plane_up_75");
    animate = Animate::create(animation);
    action = RepeatForever::create(animate);
    action->setTag(kPlaneActionTag_Forward);
    mAnimations->setObject(action, PlaneAnimation_up_75);
    
    animation = cache->getAnimation("plane_up_100");
    animate = Animate::create(animation);
    action = RepeatForever::create(animate);
    action->setTag(kPlaneActionTag_Forward);
    mAnimations->setObject(action, PlaneAnimation_up_100);
    
    FiniteTimeAction* move1 = MoveTo::create(0.1f, Point(0, getContentSize().height * 0.1f));
    FiniteTimeAction* move2 = MoveTo::create(0.2f, Point(0, -getContentSize().height * 0.1f));
    FiniteTimeAction* move3 = MoveTo::create(0.1f, Point(0, 0));
    ActionInterval* moveSeq = Sequence::create(move1, move2, move3, NULL);
    Action* repeatAction = RepeatForever::create(moveSeq);
    repeatAction->setTag(kPlaneActionTag_UpDown);
    mAnimations->setObject(repeatAction, PlaneAnimation_UpDown);
    
    
// fire
    //SpriteBatchNode* spriteBatch = SpriteBatchNode::create(String::createWithFormat("%s/fire.png", game_layer_fire_dir)->getCString());
    //mPlaneSprite->addChild(spriteBatch);

    Animate* fireAnim = ZXObjectFactory::Instanse()->createAnimation("images/game_layer/images_1", "fire", 3, 0.2f);

    mFireSprite = Sprite::createWithSpriteFrameName("fire_1.png");
    mFireSprite->setPosition(Point(mPlaneSprite->getContentSize().width * 0.35f, mPlaneSprite->getContentSize().height * 0.75f));
    mPlaneSprite->addChild(mFireSprite);

    mFireSprite->runAction(RepeatForever::create(fireAnim));
    mFireSprite->setVisible(false);

// smoke
    ParticleSystem* emitter = ZXParticleSmoke::create();
    emitter->setTexture( TextureCache::getInstance()->addImage("images/game_layer/smoke.png"));
    addChild(emitter);
    emitter->setPosition(Point(mFireSprite->getPositionX() - mFireSprite->getContentSize().width, mFireSprite->getPositionY()));
    emitter->setTag(kPlaneChildTag_Smoke);
    //emitter->setVisible(false);
    emitter->stopSystem();
}

void ZXPlane::touchEnded()
{
    runAnimation(kPlaneState_Forward);
}

void ZXPlane::applyForce(cocos2d::Point force)
{
    CCLOG("Force before = %f, Added force = %f", mForce.x, force.x);
    mForce = mForce + force;
    CCLOG("Force after = %f", mForce.x);
}

void ZXPlane::applyLinearImpulse(cocos2d::Point impulse)
{
    if (!mCanSimulate)
    {
        return;
    }
    mVelocity = mVelocity + impulse * mMass * DeviceScaling.height;
    if (impulse.y > 0)
    {
        runAnimation(kPlaneState_Up);
    }
    if (impulse.y < 0)
    {
        runAnimation(kPlaneState_Down);
    }
    if (mNeedRotation)
    {
        mTargetRotation = impulse.y < 0 ? 30 : -30;
    }
}

void ZXPlane::setLinearVelocity(cocos2d::Point vel)
{
    mVelocity = vel;
}

void ZXPlane::clearForce()
{
    mForce = Point::ZERO;
    //CCLOG("Force cleared");
}

void ZXPlane::update(float delta)
{

}

void ZXPlane::operate(float dt)
{
    solve();
    simulate(dt);
}

void ZXPlane::solve()
{
    //applyForce(mGravitation * mMass);				// гравитация
    
    //applyForce(-mVelocity * mAirFrictionConstant);	// Сопротивление ввоздуха
}

void ZXPlane::simulate(float dt)
{
    if (!mCanSimulate)
    {
        return;
    }
    static float sumdt = 0;
    sumdt += dt;
    Point power = (mGravitation * (mMass + mMassAdditional) + mForce * mMass + Point(mSpeedX,0)) * dt;
    mVelocity = mVelocity + power * DeviceScaling.height;
    
    // apply linear damping
    mVelocity.y = mVelocity.y * MAX(0, MIN(1.0f - /*dt **/ mLinearDamping /* DeviceScaling.height*/, 1));
    
    // Back Velocity limitation
    if (mVelocity.x < kSpeedXBackMax + kSpeedXBackPlus * ZXGameManager::Instance()->getGameLayer()->getSpeedFactor()) {
        mVelocity.x = kSpeedXBackMax + kSpeedXBackPlus * ZXGameManager::Instance()->getGameLayer()->getSpeedFactor();
    }
    //CCLOG("Velocity.x = %f", mVelocity.x);
    
    Point newPos = getPosition() + mVelocity /** dt*/;
    
    if (newPos.y < mLowerLimit)
    {
        newPos.y = mLowerLimit;
        mVelocity.y = 0;
    }
    if (newPos.y > mUpperLimit)
    {
        newPos.y = mUpperLimit;
        mVelocity.y = 0;
    }
    Size screenSize = Director::getInstance()->getWinSize();
    //if (newPos.x < screenSize.width * kMaxPlanePosX)
    {
        //newPos.x += mSpeedX * dt;
        if (newPos.x > screenSize.width * kMaxPlanePosX)
        {
            newPos.x = screenSize.width * kMaxPlanePosX;
            mVelocity.x = 0;
        }
    }
    this->setPosition(newPos);
    
    rotatePlane(dt);
    
    if (getPosition().x < 0)
    {
        ZXGameManager::Instance()->getGameLayer()->prepareGameOver(kGameOverStatus_LOSE);
    }
}

void ZXPlane::draw()
{
    Node::draw();
#ifdef COCOS2D_DEBUG
    DrawPrimitives::drawCircle(Point(getContentSize().width * 0.5, getContentSize().height * 0.5), mRadius, 0, 10, false);
#endif
}

Circle2d ZXPlane::getCircle()
{
    Circle2d circle;
    circle.position = getPosition();
    circle.radius = mRadius;
    return circle;
}

void ZXPlane::collideWithObjectTypeBegan(ZXCollideObject* object)
{
    switch (object->getType())
    {
        case kColliderType_Ground:
        {
            changeState(kPlaneState_OnGround);
            break;
        }
        case kColliderType_Tree:
        {
            changeState(kPlaneState_MoveBack);
            break;
        }
        case kColliderType_CloudEmpty:
        {
            changeState(kPlaneState_InCloud, object);
            break;
        }
        case kColliderType_CloudRain:
        {
            changeState(kPlaneState_Wet);
            break;
        }
        case kColliderType_CloudThunder:
        {
            changeState(kPlaneState_Burn);
            break;
        }
        case kColliderType_BonusMagnet:
        {
            changeState(kPlaneState_Magnet);
            break;
        }
            
        default:
        {
            break;
        }
    }
}

void ZXPlane::collideWithObjectType(ZXCollideObject* object)
{
    switch (object->getType())
    {
        case kColliderType_CloudEmpty:
        {
            auto itr = std::find(mDefendedObjects2.begin(), mDefendedObjects2.end(), object);
            //if (mDefendedObjects->containsObject(object))
            if (itr != mDefendedObjects2.end())
            {
                CCLOG("Object Defended!!!!!!!!!!!!!");
            }
            else
            {
                mSpeedX = kSpeedXBack * DeviceScaling.width + kSpeedXBackPlus * ZXGameManager::Instance()->getGameLayer()->getSpeedFactor();
                Action* act = mPlaneSprite->getActionByTag(kPlaneActionTag_UpDown);
                if (!act)
                {
                    act = (Action*)mAnimations->objectForKey(PlaneAnimation_UpDown);
                    mPlaneSprite->runAction(act);
                }
            }
            break;
        }
            
        default:
        {
            break;
        }
    }
}

void ZXPlane::collideWithObjectTypeEnded(ZXCollideObject* object)
{
    switch (object->getType())
    {
        case kColliderType_CloudEmpty:
        {
            CCLOG("Defended objects count BEFORE = %lu", mDefendedObjects2.size());
            auto itr = std::find(mDefendedObjects2.begin(), mDefendedObjects2.end(), object);
            if (itr != mDefendedObjects2.end())
            {
                mDefendedObjects2.erase(itr);
            }
            CCLOG("Defended objects count AFTER = %lu", mDefendedObjects2.size());
            mSpeedX = kSpeedPlaneX * DeviceScaling.width;
            mVelocity.x = 0;
            Action* act = mPlaneSprite->getActionByTag(kPlaneActionTag_UpDown);
            if (act)
            {
                mPlaneSprite->stopAction(act);
            }
            break;
        }
            
        default:
        {
            break;
        }
    }
}

void ZXPlane::moveBack()
{
    mCanSimulate = false;
    mVelocity.y = 0;
    float maxDistance = kMaxDistanceForMoveback * DeviceScaling.width;
    Size screenSize = Director::getInstance()->getWinSize();
    Point targetPos = Point(0, screenSize.height * 0.8);
    float dist = getPosition().getDistance(targetPos);
    Point pointOnLine = getPosition().lerp(targetPos, maxDistance / dist);
    //CCLOG("Target pos (%f, %f), dist = %f, Point on line: (%f, %f)", targetPos.x, targetPos.y, dist, pointOnLine.x, pointOnLine.y);
    
    ActionInterval* moveAction = MoveTo::create(DURATION_FOR_MOVE_BACK, pointOnLine);
    ActionInterval* easeAction = EaseOut::create(moveAction, 2.5);
    
	FiniteTimeAction* actionDone = CallFunc::create( CC_CALLBACK_0(ZXPlane::moveBackFinished, this));
    Sequence *seq = Sequence::create(
                                     easeAction,
                                     actionDone,
                                     NULL);
    seq->setTag(kPlaneActionTag_MoveBack);
    runAction(seq);
}

void ZXPlane::moveBackFinished()
{
    CCLOG("Move back finished!");
    mCanSimulate = true;
}

void ZXPlane::collideWithCloudRain()
{
    CCLOG("Plane wet started!!!");
    mMassAdditional = mMassAdditionalMax * DeviceScaling.width;
    mPlaneSprite->setColor(Color3B(125, 216, 213));
    FiniteTimeAction* actionDone = CallFunc::create( CC_CALLBACK_0(ZXPlane::collidingWithCloudRainFinished, this));
    Sequence *seq = Sequence::create(
                                     DelayTime::create(kPlaneStateTime_Wet),
                                     actionDone,
                                     NULL);
    seq->setTag(kPlaneActionTag_Wet);
    runAction(seq);
}

void ZXPlane::collidingWithCloudRainFinished()
{
    CCLOG("Plane wet finished!!!");
    mMassAdditional = 0;
    mPlaneSprite->setColor(Color3B::WHITE);
    removeStateFlag(kPlaneState_Wet);
}

void ZXPlane::collideWithCloudThunder()
{
    CCLOG("Plane burn started!!!");
    mBurnCount = kBurningPeriod;
    mFireSprite->setVisible(true);
    
    ParticleSystem* emitter = (ParticleSystem*)getChildByTag(kPlaneChildTag_Smoke);
    //emitter->setVisible(true);
    if(!emitter->isActive())
    {
        emitter->resetSystem();
    }
    
    schedule(schedule_selector(ZXPlane::collidingWithCloudThunderIntervalFinished), 1);
}

void ZXPlane::setFireEnabled(bool isEnable)
{
    if (isEnable)
    {
        
    }
}

void ZXPlane::collidingWithCloudThunderIntervalFinished(float dt)
{
    CCLOG("Plane burn = %d", mBurnCount);
    if(--mBurnCount == 0)
    {
        collidingWithCloudThunderFinished();
    }
    ZXGameManager::Instance()->getGameLayer()->changeEnergy(-1);
}

void ZXPlane::collidingWithCloudThunderFinished()
{
    CCLOG("Plane burn finished!!!");

    ZXGameManager::Instance()->getGameLayer()->fireOffCompleted();
    unschedule(schedule_selector(ZXPlane::collidingWithCloudThunderIntervalFinished));
    mFireSprite->setVisible(false);
    removeStateFlag(kPlaneState_Burn);

    ParticleSystem* emitter = (ParticleSystem*)getChildByTag(kPlaneChildTag_Smoke);
    //emitter->setVisible(false);
    emitter->stopSystem();
}

void ZXPlane::activateShield()
{
    CCLOG("Plane shield started!!!");
    Action* action = getActionByTag(kPlaneActionTag_Burn);
    if(action)
    {
        stopAction(action);
    }
    collidingWithCloudThunderFinished();

    action = getActionByTag(kPlaneActionTag_Wet);
    if(action)
    {
        stopAction(action);
    }
    collidingWithCloudRainFinished();

    addStateFlag(kPlaneState_Shield);
    mShieldSprite->setVisible(true);
    FiniteTimeAction* actionDone = CallFunc::create( CC_CALLBACK_0(ZXPlane::shieldTimeFinished, this));
    Sequence *seq = Sequence::create(
                                     DelayTime::create(kPlaneStateTime_Shield),
                                     actionDone,
                                     NULL);
    seq->setTag(kPlaneActionTag_Shield);
    runAction(seq);
}

void ZXPlane::shieldTimeFinished()
{
    removeStateFlag(kPlaneState_Shield);
    mShieldSprite->setVisible(false);
}

void ZXPlane::stopPlaneAndUnscheduleTimers()
{
    unscheduleAllSelectors();
}

void ZXPlane::pausePlaneAndSchedulersTimers()
{
    pauseSchedulerAndActions();
}

void ZXPlane::resumePlaneAndSchedulersTimers()
{
    resumeSchedulerAndActions();
}

void ZXPlane::changeState(ZXPlaneState state, ZXCollideObject* collideObject)
{
    //CCLOG("Enter state = %d", state);
    if (kPlaneState_Wet == state)
    {
        if (stateFlagExists(kPlaneState_Burn))
        {
            CCLOG("Current State = BURN");
            removeStateFlag(kPlaneState_Burn);
            Action* rainAction = getActionByTag(kPlaneActionTag_Burn);
            if(rainAction)
            {
                stopAction(rainAction);
            }
            collidingWithCloudThunderFinished();
        }
        else if (stateFlagExists(kPlaneState_Shield))
        {
            CCLOG("Current State = SHIELD");
            //removeStateFlag(kPlaneState_Shield);
            //mShieldSprite->setVisible(false);
        }
        else
        {
            CCLOG("New State = WET");
            addStateFlag(kPlaneState_Wet);
            collideWithCloudRain();
        }

    }
    if (kPlaneState_Burn == state)
    {
        if (stateFlagExists(kPlaneState_Wet))
        {
            CCLOG("Current State = WET");
            removeStateFlag(kPlaneState_Wet);
            Action* rainAction = getActionByTag(kPlaneActionTag_Wet);
            if(rainAction)
            {
                stopAction(rainAction);
            }
            collidingWithCloudRainFinished();
        }
        else if (stateFlagExists(kPlaneState_Shield))
        {
            CCLOG("Current State = SHIELD");
            //removeStateFlag(kPlaneState_Shield);
            //mShieldSprite->setVisible(false);
        }
        else
        {
            CCLOG("New State = BURN");
            addStateFlag(kPlaneState_Burn);
            collideWithCloudThunder();
        }
    }
    if (kPlaneState_OnGround == state)
    {
        if (stateFlagExists(kPlaneState_Shield))
        {
            CCLOG("Current State = SHIELD");
            //removeStateFlag(kPlaneState_Shield);
            //mShieldSprite->setVisible(false);
        }
         moveBack();
    }
    if (kPlaneState_MoveBack == state)
    {
        if (stateFlagExists(kPlaneState_Shield))
        {
            CCLOG("Current State = SHIELD");
            //removeStateFlag(kPlaneState_Shield);
            //mShieldSprite->setVisible(false);
        }
        else
        {
            moveBack();
        }
    }
    if (kPlaneState_InCloud == state)
    {
        if (stateFlagExists(kPlaneState_Shield))
        {
            CCLOG("Current State = SHIELD");
            //removeStateFlag(kPlaneState_Shield);
            //mShieldSprite->setVisible(false);
            if (collideObject)
            {
                mDefendedObjects2.push_back(collideObject);
                CCLOG("Defended objects count = %lu", mDefendedObjects2.size());
            }
        }
        else
        {
            //moveBack();
        }
    }
    if (kPlaneState_Shield == state)
    {
        activateShield();
    }
    if (kPlaneState_Magnet == state)
    {
        addStateFlag(kPlaneState_Magnet);
        unschedule(schedule_selector(ZXPlane::upgradeMagnetFinished));
        scheduleOnce(schedule_selector(ZXPlane::upgradeMagnetFinished), ZXGameManager::Instance()->GameParams().upgradeMagnetTime);
    }
    
    if (kPlaneState_Forward == state)
    {
        runAnimation(state);
    }
}

bool ZXPlane::applyUpgrade(int type)
{
    bool applied = false;
    switch (type) {
        case kColliderType_UpgradeDrop:
        {
            if(stateFlagExists(kPlaneState_Shield))
            {
                applied = false;
            }
            else if (stateFlagExists(kPlaneState_Burn))
            {
                applied = true;
                removeStateFlag(kPlaneState_Burn);
                Action* rainAction = getActionByTag(kPlaneActionTag_Burn);
                if(rainAction)
                {
                    stopAction(rainAction);
                }
                collidingWithCloudThunderFinished();
            }
            else
            {
                applied = true;
                addStateFlag(kPlaneState_Wet);
                collideWithCloudRain();
            }
            break;
        }
        case kColliderType_UpgradeUmbrella:
        {
            if(stateFlagExists(kPlaneState_Shield))
            {
                applied = false;
            }
            else if (stateFlagExists(kPlaneState_Wet))
            {
                applied = true;
                removeStateFlag(kPlaneState_Wet);
                Action* action = getActionByTag(kPlaneActionTag_Wet);
                if(action)
                {
                    stopAction(action);
                }
                collidingWithCloudRainFinished();
            }
            else
            {
                applied = true;
                addStateFlag(kPlaneState_Burn);
                collideWithCloudThunder();
            }
            break;
        }
        case kColliderType_UpgradeShield:
        {
            if (!stateFlagExists(kPlaneState_Shield))
            {
                applied = true;
                changeState(kPlaneState_Shield);
            }
            break;
        }
        case kColliderType_UpgradeMagnet:
        {
            changeState(kPlaneState_Magnet);
            break;
        }
            
        default:
            break;
    }
    return applied;
}

void ZXPlane::rotatePlane(float delta)
{
    if(!mNeedRotation)
    {
        mTargetRotation = 0;
    }
    int rotDir = mTargetRotation - mPlaneSprite->getRotation() > 0 ? 1 : -1;
    float newRot = mPlaneSprite->getRotation() + 20 * delta * rotDir;
    if (rotDir == 1 && newRot > mTargetRotation)
    {
        newRot = mTargetRotation;
    }
    if (rotDir == -1 && newRot < mTargetRotation)
    {
        newRot = mTargetRotation;
    }
    mPlaneSprite->setRotation(newRot);
}

void ZXPlane::runAnimation(int direction)
{
    switch (direction)
    {
        case kPlaneState_Forward:
        {
            mPlaneSprite->stopActionByTag(kPlaneActionTag_Forward);
            Action* action = (Action*)mAnimations->objectForKey(mDeformationState);
            if(action)
            {
                mPlaneSprite->runAction(action);
            }
            break;
        }
        case kPlaneState_Down:
        {
            mPlaneSprite->stopActionByTag(kPlaneActionTag_Forward);
            Action* action = (Action*)mAnimations->objectForKey(mDeformationState + 100);
            if(action)
            {
                mPlaneSprite->runAction(action);
            }
            break;
        }
        case kPlaneState_Up:
        {
            mPlaneSprite->stopActionByTag(kPlaneActionTag_Forward);
            Action* action = (Action*)mAnimations->objectForKey(mDeformationState + 200);
            if(action)
            {
                mPlaneSprite->runAction(action);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

void ZXPlane::setDeformationLevel(int energy)
{
    if(energy < 100 / 3.0f)
    {
        mDeformationState = PlaneAnimation_50;
    }
    else if(energy < 100 / 3.0f * 2)
    {
        mDeformationState = PlaneAnimation_75;
    }
    else
    {
        mDeformationState = PlaneAnimation_100;
    }
    changeState(kPlaneState_Forward);
}

float ZXPlane::getSpeedFactor()
{
    return getPositionX() / (kMaxPlanePosX * VISIBLE_SIZE.width);
}

void ZXPlane::clearStates()
{
    collidingWithCloudThunderFinished();
    collidingWithCloudRainFinished();
}

void ZXPlane::hidePlane()
{
    mPlaneSprite->setVisible(false);
}

void ZXPlane::showPlane()
{
    mPlaneSprite->setVisible(true);
}

void ZXPlane::resetRotation()
{
    auto action = RotateTo::create(1, 0);
    mPlaneSprite->runAction(action);
}

void ZXPlane::upgradeMagnetFinished(float dt)
{
    removeStateFlag(kPlaneState_Magnet);
}