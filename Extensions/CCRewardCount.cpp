//
//  CCRewardCount.cpp
//  Galaxy
//
//  Created by Tact Sky on 3/12/13.
//
//

#include "CCRewardCount.h"
#include "GWBuilding.h"
#include "GWUtilities.h"

CCRewardCount::CCRewardCount(){
    m_building=NULL;
}

CCRewardCount::~CCRewardCount(){
    CC_SAFE_RELEASE_NULL(m_building);
}

bool CCRewardCount::initWithDuration(float d, GWBuilding* building){
    bool bRet=false;
    do {
        CC_BREAK_IF(CCActionInterval::initWithDuration(d)==false);
        m_building=building;
        CC_SAFE_RETAIN(m_building);
        bRet=true;
    } while (false);
    return bRet;
}

CCRewardCount* CCRewardCount::create(float d, GWBuilding* building){
    CCRewardCount *instance=new CCRewardCount();
    if(instance && instance->initWithDuration(d, building)){
        instance->autorelease();
        return instance;
    }
    delete instance;
    instance=NULL;
    return NULL;
}

void CCRewardCount::startWithTarget(cocos2d::CCNode *pTarget){
    CCActionInterval::startWithTarget(pTarget);
}

void CCRewardCount::update(float dt){
    do{
        // change text

        CCLabelProtocol* label=dynamic_cast<CCLabelProtocol*>(m_pTarget); 
        float reward=m_building->getCollectedCurrency()->getValue();
        CCString* message=CCString::createWithFormat("+%.0f", floor(reward));
        label->setString(message->getCString());
        
    }while(false);
}
