//
//  CCNumberCount.cpp
//  Galaxy
//
//  Created by Tact Sky on 22/11/13.
//
//

#include "CCNumberCount.h"

bool CCNumberCount::initWithDuration(float duration, float currentValue, float targetValue){
    bool bRet=false;
    do {
        m_fDuration=duration;
        m_currentValue=currentValue;
        m_targetValue=targetValue;
        bRet=true;
    } while (false);
    return bRet;
}

CCNumberCount* CCNumberCount::create(float duration, float currentValue, float targetValue){
    CCNumberCount* instance=new CCNumberCount();
    if(instance && instance->initWithDuration(duration, currentValue, targetValue)){
        instance->autorelease();
        return instance;
    }
    delete instance;
    instance=NULL;
    return NULL;
}

CCObject* CCNumberCount::copyWithZone(CCZone* pZone){
    CCZone* pNewZone = NULL;
    CCNumberCount* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        //in case of being called at sub class
        pCopy = (CCNumberCount*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CCNumberCount();
        pZone = pNewZone = new CCZone(pCopy);
    }
    
    CCActionInterval::copyWithZone(pZone);
    
    
    pCopy->initWithDuration(m_fDuration, m_currentValue, m_targetValue);
    
    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

void CCNumberCount::startWithTarget(CCNode *pTarget){
    CCActionInterval::startWithTarget(pTarget);
    m_deltaValue=(m_targetValue-m_currentValue);
}

void CCNumberCount::update(float time){
    bool stop=false;
    
    if(m_pTarget){
       
        float dValue=m_deltaValue*(time/m_fDuration);
        m_currentValue+=dValue;
        if(m_deltaValue<=0 && m_currentValue<=m_targetValue){
            m_currentValue=m_targetValue;
            stop=true;
        }
        
        if(m_deltaValue>=0 && m_currentValue>=m_targetValue){
            m_currentValue=m_targetValue;
            stop=true;
        }
        const char* valueString=CCString::createWithFormat("%.0f",m_currentValue)->getCString();
        CCLabelProtocol* label=dynamic_cast<CCLabelProtocol*>(m_pTarget);
        if (label) {
            label->setString(valueString);
        }
        
        if(stop==true){
            m_pTarget->stopAllActions();
        }
    }
}
