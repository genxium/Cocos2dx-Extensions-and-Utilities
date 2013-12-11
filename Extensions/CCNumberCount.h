//
//  CCNumberCount.h
//  Galaxy
//
//  Created by Tact Sky on 22/11/13.
//
//

#ifndef __Galaxy__CCNumberCount__
#define __Galaxy__CCNumberCount__

#include <iostream>
#include "cocos2d.h"
USING_NS_CC;

class CCNumberCount: public CCActionInterval{
public:
    bool initWithDuration(float duration, float currentValue, float targetValue);
    
    virtual CCObject* copyWithZone(CCZone* pZone);
    virtual void startWithTarget(CCNode *pTarget);
    virtual void update(float time);
    
public:
    static CCNumberCount* create(float duration, float currentValue, float targetValue);
    
private:
    float m_currentValue;
    float m_targetValue;
    float m_deltaValue;
};

#endif /* defined(__Galaxy__CCNumberCount__) */
