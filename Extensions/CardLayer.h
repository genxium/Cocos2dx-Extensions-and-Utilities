//
//  CardLayer.h
//  

#ifndef __CardLayer__
#define __CardLayer__

#include "cocos2d.h"
USING_NS_CC;

#define kCard1Name "buyfeature-bg-blue.png"
#define kCard2Name "feature-item-bg.png"

#define kCard1NoLightName "buyfeature-item-bg-nolight.png"

class GWVehicle;
class GWFeatureItem;
class GWRobot;

class GWBuilding;
class GWWorld;

class CardLayer: public CCLayerRGBA{
public:
    virtual ~CardLayer();
    
    /* for vehicles */
    virtual bool initWithVehicle(GWVehicle* vehicle, GWBuilding* building, GWWorld* world);
    static CardLayer* createWithVehicle(GWVehicle* vehicle, GWBuilding* building, GWWorld* world);
    /*
     
     // TODO...
    virtual bool initWithFeatureItem(GWFeatureItem* featureItem, GWBuilding* building, GWWorld* world);
    static CardLayer* createWithFeatureItem(GWFeatureItem* featureItem, GWBuilding* building, GWWorld* world);
    
    virtual bool initWithRobot(GWRobot* robot, GWBuilding* building, GWWorld* world);
    static CardLayer* createWithRobot(GWRobot* robot, GWBuilding* building, GWWorld* world);
     */
    
public:
    void setGray();
    void setProgressPercentage(float percentage);
    void flipWithCallback(CCObject* pTarget, SEL_CallFuncN pSelector);
    void removePriceAndRewardIndicators();
    void addRequirementText();
    
private:
    CCSprite* generateFrontalCard();
    CCSprite* generateBackCard();
    
    // general functions
    void attachCardToLayer();
    void triggerSelector();
    void cleanCard();
    
    // flip action
    void flip();
    void setFlipCallback(CCObject* pTarget, SEL_CallFuncN pSelector);
    void switchDisplayInstantly();
    CCAction* pseudoFlipAction(float firstDuration, float secondDuration);
    
CC_SYNTHESIZE(bool, m_isDisabled, IsDisabled); // availability indicator
CC_SYNTHESIZE(bool, m_isFrontal, IsFrontal); // frontal orientation indicator
CC_SYNTHESIZE(CCLabelProtocol*, m_titleLabel, TitleLabel); // card title
CC_SYNTHESIZE(CCLabelProtocol*, m_subTitleLabel, SubTitleLabel); // card sub-title
CC_SYNTHESIZE(CCLabelProtocol*, m_descriptionLabel, DescriptionLabel); // card description on the back
CC_SYNTHESIZE(CCSprite*, m_background, Background); // card background
CC_SYNTHESIZE(CCSprite*, m_mainImage, MainImage); // card main image
CC_SYNTHESIZE(CCProgressTimer*, m_progressBar, ProgressBar); // card progress bar
CC_SYNTHESIZE(CCNode*, m_bottomIcons, BottomIcons);
CC_SYNTHESIZE(CCSprite*, m_priceIcon, PriceIcon);
CC_SYNTHESIZE(CCLabelProtocol*, m_priceLabel, PriceLabel);
CC_SYNTHESIZE(CCSprite*, m_buyButton, BuyButton);
CC_SYNTHESIZE(CCSprite*, m_xpBox, XpBox);
CC_SYNTHESIZE(CCSprite*, m_xpIcon, XpIcon);
CC_SYNTHESIZE(CCLabelProtocol*, m_xpLabel, XpLabel);
    
protected:
    GWVehicle* m_vehicle;
    GWBuilding* m_building;
    GWWorld* m_world;
    
    CCObject* m_pTarget;
    SEL_CallFuncN m_pSelector;
};

#endif /* defined(__CardLayer__) */
