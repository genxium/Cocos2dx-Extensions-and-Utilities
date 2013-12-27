//
//  CardLayer.cpp
//

#include "CardLayer.h"
#include "GWVehicle.h"
#include "GWVehicleDetailInfo.h"
#include "GWBuilding.h"
#include "GWWorld.h"
#include "GWGlobalDef.h"
#include "ImageUtils.h"
#include "GWUtilities.h"

CardLayer::~CardLayer(){
    CC_SAFE_RELEASE_NULL(m_vehicle);
    CC_SAFE_RELEASE_NULL(m_building);
    CC_SAFE_RELEASE_NULL(m_world);
    CC_SAFE_RELEASE_NULL(m_pTarget);
}

bool CardLayer::initWithVehicle(GWVehicle* vehicle, GWBuilding* building, GWWorld* world){
    bool bRet=false;
    do {
        bRet=true;
        m_isDisabled=false;
        m_isFrontal=true;
        m_background=NULL;
        m_mainImage=NULL;
        m_titleLabel=NULL;
        m_subTitleLabel=NULL;
        m_bottomIcons=NULL;
        m_priceIcon=NULL;
        m_priceLabel=NULL;
        m_buyButton=NULL;
        m_xpBox=NULL;
        m_xpIcon=NULL;
        m_xpLabel=NULL;
        
        m_pTarget=NULL;
        m_pSelector=NULL;
        
        m_vehicle=vehicle;
        CC_SAFE_RETAIN(m_vehicle);
        m_building=building;
        CC_SAFE_RETAIN(m_building);
        m_world=world;
        CC_SAFE_RETAIN(m_world);
        
        m_background=generateFrontalCard();
        attachCardToLayer();
        
    } while (false);
    return bRet;
}

CardLayer* CardLayer::createWithVehicle(GWVehicle* vehicle, GWBuilding* building, GWWorld* world){
    CardLayer* instance=new CardLayer();
    if(instance && instance->initWithVehicle(vehicle, building, world)){
        instance->autorelease();
        return instance;
    }
    delete instance;
    instance=NULL;
    return NULL;
}

CCSprite* CardLayer::generateFrontalCard(){
    
    GWVehicleDetailInfo* detailInfo = m_vehicle->getDetailInfo();
    
    // card background
    CCSprite* card=CCSprite::create(kCard1Name);
    CCSize cardSize=card->getContentSize();
    
    // card title
    CCString* displayName=detailInfo->getDisplayName();
    m_titleLabel = CCLabelBMFont::create(displayName->getCString(), kFontKankinShad, 0.9*cardSize.width, kCCTextAlignmentLeft);
    CCLabelBMFont* titleBMFont=(CCLabelBMFont*)m_titleLabel;
    titleBMFont->setScale(0.2);
    titleBMFont->setPosition(ccp(cardSize.width*0.35, cardSize.height*0.9));
    card->addChild(titleBMFont);
    
    // main image
    CCString* imageName=m_vehicle->getImageName();
    CCString* imageNameComplete=CCString::createWithFormat("%s.png", imageName->getCString());
    m_mainImage=CCSprite::create(imageNameComplete->getCString());
    m_mainImage->setPosition(ccp(cardSize.width*0.5, cardSize.height*0.5));
    card->addChild(m_mainImage);
    
    // price bar
    m_buyButton=CCSprite::create("btn-buybg.png");
    m_buyButton->setPosition(ccp(cardSize.width*0.5, cardSize.height*0.12));
    card->addChild(m_buyButton);
    
    // price icon & label
    GWCurrency* cost=detailInfo->getCost();
    CCString* iconName=cost->getSmallIconName();
    
    m_priceIcon=CCSprite::create(iconName->getCString());
    m_priceIcon->setPosition(ccp(cardSize.width*0.3, cardSize.height*0.12));
   
    CCString* priceValueStr=CCString::createWithFormat("%.0f", cost->getValue());
    m_priceLabel=CCLabelTTF::create(priceValueStr->getCString(), kFontKankin, GWUtilities::getFontSizeByValueAndBasicSize(cost->getValue(), 15), CCSize(cardSize.width, cardSize.height*0.2), kCCTextAlignmentLeft, kCCVerticalTextAlignmentTop);
    ((CCLabelTTF*)m_priceLabel)->setAnchorPoint(ccp(0, 1.0));
    ((CCLabelTTF*)m_priceLabel)->setPosition(ccp(cardSize.width*0.45, cardSize.height*0.14));
    
    card->addChild(m_priceIcon);
    card->addChild(((CCLabelTTF*)m_priceLabel));
    
    // reward icon & label
    
    m_xpBox=CCSprite::create("addbuilding-stats-box.png");
    m_xpIcon=CCSprite::create("icon-xp.png");
    CCSize boxSize=m_xpBox->getContentSize();
    float rewardValue=m_vehicle->getDetailInfo()->getReward()->getValue();
    m_xpLabel=CCLabelTTF::create(CCString::createWithFormat("%.0f", rewardValue)->getCString(), kFontKankin, GWUtilities::getFontSizeByValueAndBasicSize(rewardValue, 12), CCSize(boxSize.width*0.8, boxSize.height), kCCTextAlignmentRight, kCCVerticalTextAlignmentCenter);
    ImageUtils::addChildByFrameworkRatio((CCLabelTTF*)m_xpLabel, m_xpBox, 0, -0.2);
    ImageUtils::addChildByFrameworkRatio(m_xpIcon, m_xpBox, -0.5, 0);
    ImageUtils::addChildByFrameworkRatio(m_xpBox, card, -0.55, -0.5);
    m_xpIcon->setScale(0.8);
    return card;
}

CCSprite* CardLayer::generateBackCard(){
    GWVehicleDetailInfo* detailInfo = m_vehicle->getDetailInfo();
    
    // card background
    CCSprite* card=CCSprite::create(kCard1NoLightName);
    CCSize cardSize=card->getContentSize();
    
    // card title
    CCString* displayName=detailInfo->getDisplayName();
    m_titleLabel = CCLabelBMFont::create(displayName->getCString(), kFontKankinShad, 0.9*cardSize.width, kCCTextAlignmentLeft);
    CCLabelBMFont* titleBMFont=(CCLabelBMFont*)m_titleLabel;
    titleBMFont->setScale(0.2);
    titleBMFont->setPosition(ccp(cardSize.width*0.35, cardSize.height*0.9));
    card->addChild(titleBMFont);
    
    // Description
    CCString* description=detailInfo->getDescription();
    m_descriptionLabel = CCLabelTTF::create(description->getCString(), kFontKankin, 12, CCSize(cardSize.width*0.8, cardSize.height*0.6), kCCTextAlignmentLeft);
    CCLabelTTF *descriptionTTF=(CCLabelTTF*)m_descriptionLabel;
    descriptionTTF->setPosition(ccp(cardSize.width*0.5, cardSize.height*0.5));
    card->addChild(descriptionTTF);
    
    return card;
}

void CardLayer::setGray(){
    
    m_isDisabled=true;
    
    if(m_isFrontal){
        removePriceAndRewardIndicators();
        addRequirementText();
        if(m_mainImage){
            CCString* imageName=m_vehicle->getImageName();
            CCString* imageNameComplete=CCString::createWithFormat("%s.png", imageName->getCString());
            CCTexture2D* grayMainImage=ImageUtils::getGrayTexture(imageNameComplete->getCString());
            m_mainImage->setTexture(grayMainImage);
            m_mainImage->setFlipY(true);
        }
        if(m_background){
            CCTexture2D* grayBackground=ImageUtils::getGrayTexture(kCard1Name);
            m_background->setTexture(grayBackground);
            m_background->setFlipY(true);
        }
    } else{
        if(m_background){
            CCTexture2D* grayBackground=ImageUtils::getGrayTexture(kCard1NoLightName);
            m_background->setTexture(grayBackground);
            m_background->setFlipY(true);
        }
    }
    
}

void CardLayer::setProgressPercentage(float percentage){
    if(m_progressBar){
        m_progressBar->setPercentage(percentage);
    }
}

void CardLayer::flipWithCallback(cocos2d::CCObject *pTarget, SEL_CallFuncN pSelector){
    setFlipCallback(pTarget, pSelector);
    flip();
}

void CardLayer::flip(){
    CCAction* flipAction=pseudoFlipAction(0.5, 0.5);
    this->runAction(flipAction);
}

void CardLayer::setFlipCallback(cocos2d::CCObject *pTarget, SEL_CallFuncN pSelector){
    if(m_pTarget){
        CC_SAFE_RELEASE(m_pTarget);
        m_pTarget=NULL;
        m_pSelector=NULL;
    }
    m_pTarget=pTarget;
    CC_SAFE_RETAIN(m_pTarget);
    
    m_pSelector=pSelector;
}

void CardLayer::switchDisplayInstantly(){
    m_isFrontal=!m_isFrontal;
    cleanCard();
    do{
        if (m_isFrontal) {
            m_background=generateFrontalCard();
        }else{
            m_background=generateBackCard();
        }
        attachCardToLayer();
        bool isLocked=(m_vehicle->getDetailInfo()->getRequiredLevel()>m_world->getXp()->getLevel());
        if(isLocked) {
            setGray();
        }
    }while(false);
}

CCAction* CardLayer::pseudoFlipAction(float firstDuration, float secondDuration){
    CCScaleTo* shrink = CCScaleTo::create(firstDuration, 0, 1.0);
    CCCallFunc* switchDisplayAction= CCCallFunc::create(this, callfunc_selector(CardLayer::switchDisplayInstantly));
    CCScaleTo* expand = CCScaleTo::create(secondDuration, 1.0, 1.0);
    CCCallFunc* selectorAction= CCCallFunc::create(this, callfunc_selector(CardLayer::triggerSelector));
    return CCSequence::create(shrink, switchDisplayAction, expand, selectorAction, NULL);
}

void CardLayer::attachCardToLayer(){
    do {
        CC_BREAK_IF(m_background==NULL);
        CCSize backgroundSize=m_background->getContentSize();
        this->setContentSize(backgroundSize);
        m_background->setAnchorPoint(CCPointZero);
        m_background->setPosition(CCPointZero);
        this->addChild(m_background);
        
        bool isLocked=(m_vehicle->getDetailInfo()->getRequiredLevel()>m_world->getXp()->getLevel());
        if(isLocked) {
            setGray();
        }
    } while (false);
}

void CardLayer::triggerSelector(){
    if(m_pTarget!=NULL && m_pSelector!=NULL){
        (m_pTarget->*m_pSelector)(this);
        if(m_pTarget!= NULL){
            CC_SAFE_RELEASE(m_pTarget);
            m_pTarget=NULL;
            m_pSelector=NULL;
        }
    }
}

void CardLayer::cleanCard(){
    m_background->removeAllChildren();
    this->removeAllChildren();
    m_background=NULL;
    m_mainImage=NULL;
    m_titleLabel=NULL;
    m_subTitleLabel=NULL;
    m_bottomIcons=NULL;
    m_priceIcon=NULL;
    m_priceLabel=NULL;
    m_buyButton=NULL;
}

void CardLayer::removePriceAndRewardIndicators(){
    if(m_priceIcon){
        m_priceIcon->removeFromParent();
        m_priceIcon=NULL;
    }
    if(m_priceLabel){
        ((CCLabelTTF*)m_priceLabel)->removeFromParent();
        m_priceLabel=NULL;
    }
    
    if(m_buyButton){
        m_buyButton->removeFromParent();
        m_buyButton=NULL;
    }
    
    if(m_xpLabel){
        ((CCLabelTTF*)m_xpLabel)->removeFromParent();
        m_xpLabel=NULL;
    }
    
    if(m_xpIcon){
        m_xpIcon->removeFromParent();
        m_xpIcon=NULL;
    }
    
    if(m_xpBox){
        m_xpBox->removeFromParent();
        m_xpBox=NULL;
    }
}

void CardLayer::addRequirementText(){
    do{
        CC_BREAK_IF(m_background==NULL);
        int requiredLevel=m_vehicle->getDetailInfo()->getRequiredLevel();
        CCString* requirementStr=CCString::createWithFormat("Required world level %d to unlock", requiredLevel);
        CCLabelTTF* requirementLabel=CCLabelTTF::create(requirementStr->getCString(), kFontKankin, 8);
        requirementLabel->setColor(ccRED);
        ImageUtils::addChildByFrameworkRatio(requirementLabel, m_background, 0, -0.8);
    }while(false);
}
