#ifndef __ImageUtils__
#define __ImageUtils__

#include <iostream>
#include "cocos2d.h"
#include <string>
using namespace std;
USING_NS_CC;

class ImageUtils : public CCObject {
public:
    
    static CCSprite* getSpriteFromImage(CCImage* pCCImage);
    static CCSprite* getGraySprite(const char*);
    static CCSprite* getGraySprite(CCSprite * sprite, CCRect rect = CCRectZero );
    static CCSprite* getGraySpriteWithTexture(CCTexture2D*, const char*);
    static CCSprite* getBrighterSprite(CCSprite* sprite, float increasedPercentage);
    static CCTexture2D* getBrighterTexture(CCTexture2D* src, float increasedPercentage);
    
    static CCSprite* changeRGBGradientSpriteFromFile(const char* path, ccColor3B colorFrom, ccColor3B colorTo);
    static CCNode* createStrokeForSprite(CCSprite* label, float size, ccColor3B color, GLubyte opacity);
    static CCNode* createLabelWithStroke(CCString* content, CCSize labelSize, float baseFontSize, const char* fontName="", float strokeSize=1, ccColor3B color=ccBLACK, GLubyte opacity=200, bool isAutoSized=true);
    
    static CCTexture2D* getGrayTexture(const char* path);
    static CCRect calculateRect(CCSprite* spr, CCRect rect);
    static CCPoint getPositionByFrameworkRatio(CCNode* parent, float xRatio, float yRatio);
    static void addChildByFrameworkRatio(CCNode* child, CCNode* parent, float xRatio, float yRatio);
    static CCAction* toggleScaling(float firstDuration, float firstScale, float secondDuration, float secondScale);
    static void dimScreen( CCLayer * layer );
    static void undimScreen( CCLayer * layer );
    
private:
    static ImageUtils* shareImageUtils();
    void removeCache();
    
    static CCRenderTexture* createStroke(CCSprite* label, float size, ccColor3B color, GLubyte opacity);
    
    static CCImage* changeRGBGradient(CCTexture2D* src, ccColor3B colorFrom, ccColor3B colorTo);
    static CCImage* rgbToGray(CCTexture2D* src);
    static void rgbToGray(CCImage* image);
    static CCImage* increasedBrightness(CCTexture2D* src, float increasedPercentage);
    
    void removeCache(float dt);
    static CCSprite* m_label;
    static CCNode* m_strokeCache;
    static CCSprite* m_label2;
    static CCNode* m_strokeCache2;
};
#endif
