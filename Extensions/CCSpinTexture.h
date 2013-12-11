//  CCSpinTexture.h

#ifndef __CCSpinTexture__
#define __CCSpinTexture__

#include "cocos2d.h"
USING_NS_CC;

class CCSpinTexture: public CCActionInterval{
public:
    CCSpinTexture();
    virtual ~CCSpinTexture();
    virtual bool initWithDuration(float d, CCSize rectSize, CCPoint offset, float cycleTime, CCTexture2D *tex);
    static CCSpinTexture* create(float d, CCSize rectSize, CCPoint offset, float cycleTime, CCTexture2D *tex);
    
    virtual void startWithTarget(CCNode *pTarget);
    virtual void update(float dt);
    
protected:
    CCSize m_rectSize;
    CCPoint m_offset;
    float m_cycleTime;
    CCTexture2D *m_tex;
    
    // derived parameters
    float m_speed;
};
#endif
