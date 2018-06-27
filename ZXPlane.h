//
//  Plane.h
//  PaperPlane
//
//  Created by Zolkin on 9/26/13.
//
//

#ifndef __PaperPlane__Plane__
#define __PaperPlane__Plane__

#include "cocos2d.h"
#include "GeometryExt.h"

class ZXCollideObject;

enum ZXPlaneState
{
    kPlaneState_None    = 0x0001,
    kPlaneState_Burn    = 0x0002,
    kPlaneState_Wet     = 0x0004,
    kPlaneState_Shield  = 0x0008,
    kPlaneState_MoveBack= 0x0010,
    kPlaneState_InCloud = 0x0020,
    kPlaneState_OnGround= 0x0040,
    kPlaneState_Forward = 0x0080,
    kPlaneState_Up      = 0x0100,
    kPlaneState_Down    = 0x0200,
    kPlaneState_Magnet  = 0x0400,
};

enum ZXPlaneAnimation
{
    PlaneAnimation_50 = 1,
    PlaneAnimation_75,
    PlaneAnimation_100,
    PlaneAnimation_UpDown,
    PlaneAnimation_down_50 = 101,
    PlaneAnimation_down_75,
    PlaneAnimation_down_100,
    PlaneAnimation_up_50 = 201,
    PlaneAnimation_up_75,
    PlaneAnimation_up_100,
};

class ZXPlane : public cocos2d::Node
{
public:
    virtual ~ZXPlane();
    static ZXPlane* create();
    void loadParameters();
    void createAnimations();
    void applyForce(cocos2d::Point force);
    void applyLinearImpulse(cocos2d::Point impulse);
    void setLinearVelocity(cocos2d::Point vel);
    void clearForce();
    void operate(float dt);
    Circle2d getCircle();
    void collideWithObjectTypeBegan(ZXCollideObject* object);
    void collideWithObjectType(ZXCollideObject* object);
    void collideWithObjectTypeEnded(ZXCollideObject* object);
    bool applyUpgrade(int type);
    void stopPlaneAndUnscheduleTimers();
    void pausePlaneAndSchedulersTimers();
    void resumePlaneAndSchedulersTimers();
    void setNeedRotation(bool needRot) { mNeedRotation = needRot; }
    void runAnimation(int direction); // direction = kPlaneState_Forward - forward moving, kPlaneState_Up - up moving, kPlaneState_Down - down moving
    void setDeformationLevel(int level);
    float getSpeedFactor(); // 0...1 возвращает относительную позицию между крайними точками положения самолета на экране (от 0 до kMaxPlanePosX). Позиция 0 = 0, kMaxPlanePosX = 1
    void touchEnded();
    void clearStates();
    void hidePlane();
    void showPlane();
    void resetRotation();
    bool stateFlagExists(ZXPlaneState flag) { return (mStateFlags & flag) == flag; }
    
private:
    virtual bool init();
    virtual void update(float delta);
    void simulate(float dt);
    virtual void draw();
    void correctWithLimits(cocos2d::Point& point);
    void moveBack();
    void moveBackFinished();
    void collideWithCloudRain();
    void collidingWithCloudRainFinished();
    void collideWithCloudThunder();
    void collidingWithCloudThunderIntervalFinished(float dt);
    void collidingWithCloudThunderFinished();
    void activateShield();
    void shieldTimeFinished();
    void changeState(ZXPlaneState state, ZXCollideObject* collideObject = NULL);
    void addStateFlag(ZXPlaneState flag) { mStateFlags |= flag; }
    void removeStateFlag(ZXPlaneState flag) { mStateFlags &= ~flag; }
    void rotatePlane(float delta);
    void runCollideAnimation(ZXPlaneState state);
    void upgradeMagnetFinished(float dt);
    void nonGravitationFinished(float dt);
    
    cocos2d::Sprite* mPlaneSprite;
    
    float mMass;
    float mMassAdditional;
    float mMassAdditionalMax;
    cocos2d::Point mVelocity;
    cocos2d::Point mGravitation;
    cocos2d::Point mForce;
    float mLinearDamping;
    bool mCanSimulate;
    float mSpeedX; // для возвращения в крайнюю правуую точку после смещения назад
    float mTargetRotation;
    float mNeedRotation;
    
    float mAirFrictionConstant;
    float mLowerLimit;
    float mUpperLimit;
    float mRadius;
    int mBurnCount;
    
    int mStateFlags;
    
    cocos2d::Sprite* mShieldSprite;
    cocos2d::Dictionary* mAnimations;
    ZXPlaneAnimation mDeformationState;
    cocos2d::Action* mAnimationInCloud;
    cocos2d::Sprite* mFireSprite;
    
    std::list<ZXCollideObject*> mDefendedObjects2;
};

#endif /* defined(__PaperPlane__Plane__) */
