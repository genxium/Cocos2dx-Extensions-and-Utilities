#include "CCSpinTexture.h"

static CCNode* mergeSpriteNodeVertically(CCSprite *sprUpside, CCSprite *sprDownside){
    CCNode* res=CCNode::create();
    res->setContentSize(CCSize(0,0));
    do {
        CC_BREAK_IF(sprUpside==NULL || sprDownside==NULL);
        CC_BREAK_IF(sprUpside->getContentSize().width!=sprDownside->getContentSize().width);
        CCSize upsideSize=sprUpside->getContentSize(), downsideSize=sprDownside->getContentSize();
        CCSize nodeSize=CCSize(upsideSize.width, upsideSize.height+downsideSize.height);
        res->setContentSize(nodeSize);
        sprUpside->setAnchorPoint(CCPointZero);
        sprDownside->setAnchorPoint(CCPointZero);
        
        sprUpside->setPosition(ccp(0, downsideSize.height));
        sprDownside->setPosition(CCPointZero);
        
        res->addChild(sprDownside);
        res->addChild(sprUpside);
    } while (false);
    return res;
}

CCSpinTexture::CCSpinTexture(){
    m_tex=NULL;
    m_rectSize=CCSize(0,0);
    m_offset=CCPointZero;
    m_cycleTime=0;
    m_speed=0;
}

CCSpinTexture::~CCSpinTexture(){
    
}

bool CCSpinTexture::initWithDuration(float d, CCSize rectSize, cocos2d::CCPoint offset, float cycleTime, cocos2d::CCTexture2D *tex){
    bool bRet=false;
    do {
        CC_BREAK_IF(CCActionInterval::initWithDuration(d)==false);
        m_rectSize=rectSize;
        m_offset=offset;
        m_cycleTime=cycleTime;
        m_tex=tex;
        m_speed=m_tex->getContentSize().height/m_cycleTime;
        bRet=true;
    } while (false);
    return bRet;
}

CCSpinTexture* CCSpinTexture::create(float d, CCSize rectSize, cocos2d::CCPoint offset, float cycleTime, cocos2d::CCTexture2D *tex){
    CCSpinTexture *instance=new CCSpinTexture();
    if(instance && instance->initWithDuration(d, rectSize, offset, cycleTime, tex)){
        instance->autorelease();
        return instance;
    }
    delete instance;
    instance=NULL;
    return NULL;
}

void CCSpinTexture::startWithTarget(CCNode *pTarget){
    CCActionInterval::startWithTarget(pTarget);
}

void CCSpinTexture::update(float dt){
    m_pTarget->removeAllChildren();
    CCSize texSize=m_tex->getContentSize();
    float newY=fmod(m_offset.y+m_speed*dt, texSize.height);
    m_offset.y=newY;
    
    if(m_offset.y+m_rectSize.height<=m_tex->getContentSize().height){
        CCRect rect(m_offset.x, m_offset.y, m_rectSize.width, m_rectSize.height);
        CCSprite *spr=CCSprite::createWithTexture(m_tex, rect);
        spr->setAnchorPoint(CCPointZero);
        m_pTarget->addChild(spr);
    } else{
        CCRect rectUpside(m_offset.x, m_offset.y, texSize.width, texSize.height-m_offset.y);
        CCRect rectDownside(0, 0, texSize.width, m_offset.y+m_rectSize.height-texSize.height);
        CCSprite *sprUpside=CCSprite::createWithTexture(m_tex, rectUpside);
        CCSprite *sprDownside=CCSprite::createWithTexture(m_tex, rectDownside);
        CCNode *mergedSprite=mergeSpriteNodeVertically(sprUpside, sprDownside);
        m_pTarget->addChild(mergedSprite);
    }
}
