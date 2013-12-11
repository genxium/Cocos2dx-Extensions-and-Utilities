//
//  CCRewardCount.h
//  Galaxy
//
//  Created by Tact Sky on 3/12/13.
//
//

#ifndef __Galaxy__CCRewardCount__
#define __Galaxy__CCRewardCount__

#include "cocos2d.h"
USING_NS_CC;

class GWBuilding;

class CCRewardCount: public CCActionInterval{
public:
    CCRewardCount();
    virtual ~CCRewardCount();
    virtual bool initWithDuration(float d=1.0, GWBuilding* building=NULL);
    static CCRewardCount* create(float d=1.0, GWBuilding* building=NULL);
    
    virtual void startWithTarget(CCNode *pTarget);
    virtual void update(float dt);
    
private:
    GWBuilding* m_building;
};

#endif /* defined(__Galaxy__CCRewardCount__) */
