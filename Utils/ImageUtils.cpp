#include "ImageUtils.h"
#include "ArithmeticUtils.h"
#define kGrayPrefix "gray-"
#define kGradientPrefix "gradient-"
#define kBrightPrefix "bright-"

#define kTagDimBackground (6789)

CCSprite* ImageUtils::m_label = NULL;
CCSprite* ImageUtils::m_label2 = NULL;
CCNode* ImageUtils::m_strokeCache = NULL;
CCNode* ImageUtils::m_strokeCache2 = NULL;

static ImageUtils* s_imageUtils = NULL;

ImageUtils* ImageUtils::shareImageUtils()
{
    if(s_imageUtils==NULL)
    {
        s_imageUtils = new ImageUtils();
    }
    
    return s_imageUtils;
}


CCSprite* ImageUtils::getSpriteFromImage(CCImage* pCCImage)
{
    CCTexture2D* pTexture = new CCTexture2D();
    if (pTexture && pTexture->initWithImage(pCCImage)) {
        pTexture->autorelease();
        CCSprite* pSprite = CCSprite::createWithTexture(pTexture);
        return pSprite;
    }
    else
    {
        delete pTexture;
        pTexture = NULL;
        return NULL;
    }
    return NULL;
}

CCNode* ImageUtils::createStrokeForSprite(cocos2d::CCSprite *label, float size, ccColor3B color, GLubyte opacity)
{
    CCRenderTexture* render=createStroke(label, size, color, opacity);
    CCNode* retSpr=render;
    return retSpr;
}

CCNode* ImageUtils::createLabelWithStroke(CCString *content, CCSize labelSize, float baseFontSize, const char* fontName, float strokeSize, ccColor3B color, GLubyte opacity, bool isAutoSized){
    float fontSize=baseFontSize;
    if(isAutoSized){
        do{
            bool isAllDigits=true;
            const char* s=content->getCString();
            long length=strlen(s);
            for(int i=0;i<length;i++){
                if (!isdigit(s[i])) {
                    isAllDigits=false;
                    break;
                }
            }
            CC_BREAK_IF(isAllDigits==false); // use baseFontSize
            float value=atof(content->getCString());
            fontSize= ArithmeticUtils::getFontSizeByValueAndBasicSize(value, baseFontSize);
        }while(false);
    }
    CCLabelTTF* label=CCLabelTTF::create(content->getCString(), fontName, baseFontSize, labelSize, kCCTextAlignmentLeft);
    CCNode* stroke=createStrokeForSprite(label, strokeSize, color, opacity);
    CCPoint pos = ccp(labelSize.width*0.5, labelSize.height*0.5);
    stroke->setPosition(pos);
    label->addChild(stroke, label->getZOrder()-1);
    return label;
}

void ImageUtils::removeCache(){
    CC_SAFE_RELEASE_NULL(m_label);
    CC_SAFE_RELEASE_NULL(m_strokeCache);
    CC_SAFE_RELEASE_NULL(m_label2);
    CC_SAFE_RELEASE_NULL(m_strokeCache2);
}

CCSprite* ImageUtils::getGraySprite(CCSprite * sprite, CCRect rect )
{
    if ( sprite == NULL ) {
        return NULL;
    }
    if ( rect.size.width == 0 && rect.size.height == 0 ) {
        rect.size.width = sprite->getTexture()->getContentSize().width;
        rect.size.height = sprite->getTexture()->getContentSize().height;
    }
    
    CCRenderTexture* rt = CCRenderTexture::create(rect.size.width,
                                                  rect.size.height
                                                  );
    
    CCPoint originalAnchor = sprite->getAnchorPoint();
    sprite->setAnchorPoint(CCPointZero);
    sprite->setPosition(ccp( - rect.origin.x, - rect.origin.y ));
    
    rt->beginWithClear(0, 0, 0, 0);
    sprite->visit();
    rt->end();
    
    sprite->setAnchorPoint(originalAnchor);
    
    CCImage* image=rt->newCCImage();
    rgbToGray(image);
    CCSprite* spr=ImageUtils::getSpriteFromImage(image);
    CC_SAFE_DELETE(image);
    return spr;
}

CCSprite* ImageUtils::getGraySprite(const char* path) {
    const char* grayPrefix = kGrayPrefix;
    CCTexture2D* src=CCTextureCache::sharedTextureCache()->addImage(path);
    CCImage* image=ImageUtils::rgbToGray(src);
    CCSprite* spr=ImageUtils::getSpriteFromImage(image);
    CCTextureCache::sharedTextureCache()->addUIImage(image, CCString::createWithFormat("%s%s",grayPrefix,path)->getCString());
    CC_SAFE_DELETE(image);
    spr->setFlipY(true);
    return spr;
}

CCSprite* ImageUtils::getGraySpriteWithTexture(cocos2d::CCTexture2D * src, const char * path){
    const char* grayPrefix = kGrayPrefix;
    CCImage* image=ImageUtils::rgbToGray(src);
    CCSprite* spr=ImageUtils::getSpriteFromImage(image);
    CCTextureCache::sharedTextureCache()->addUIImage(image, CCString::createWithFormat("%s%s",grayPrefix,path)->getCString());
    CC_SAFE_DELETE(image);
    return spr;
}

CCTexture2D* ImageUtils::getGrayTexture(const char* path){
    const char* grayPrefix = kGrayPrefix;
    CCString* key=CCString::createWithFormat("%s%s",grayPrefix,path);
    CCTexture2D* retTexture=CCTextureCache::sharedTextureCache()->textureForKey(key->getCString());
    if(retTexture==NULL){
        CCTexture2D* src=CCTextureCache::sharedTextureCache()->addImage(path);
        CCImage* image=ImageUtils::rgbToGray(src);
        retTexture=CCTextureCache::sharedTextureCache()->addUIImage(image, key->getCString());
        CC_SAFE_DELETE(image);
    }
    return retTexture;
}

CCSprite* ImageUtils::getBrighterSprite(cocos2d::CCSprite *sprite, float increasedBrightness){
    if ( sprite == NULL ) {
        return NULL;
    }
    CCRenderTexture* rt = CCRenderTexture::create(sprite->boundingBox().size.width,
                                                  sprite->boundingBox().size.height
                                                  );
    CCPoint originalAnchor = sprite->getAnchorPoint();
    sprite->setAnchorPoint(CCPointZero);
    
    rt->beginWithClear(0, 0, 0, 0);
    sprite->visit();
    rt->end();
    
    sprite->setAnchorPoint(originalAnchor);
    sprite->setFlipY(true);
    
    CCImage* image=ImageUtils::increasedBrightness(rt->getSprite()->getTexture(), increasedBrightness);
    CCSprite* spr=ImageUtils::getSpriteFromImage(image);
    CC_SAFE_DELETE(image);
    return spr;
}

CCTexture2D* ImageUtils::getBrighterTexture(CCTexture2D* src, float increasedBrightness){
    CCImage* image=ImageUtils::increasedBrightness(src, increasedBrightness);
    CCString* key=CCString::createWithFormat("%sannoymous.png", kBrightPrefix);
    return CCTextureCache::sharedTextureCache()->addUIImage(image, key->getCString());
}

CCSprite* ImageUtils::changeRGBGradientSpriteFromFile(const char *path, ccColor3B colorFrom, ccColor3B colorTo){
    const char* gradientPrefix = kGradientPrefix;
    CCTexture2D* src=CCTextureCache::sharedTextureCache()->addImage(path);
    CCImage* image=ImageUtils::changeRGBGradient(src, colorFrom, colorTo);
    CCSprite* spr=getSpriteFromImage(image);
    CCTextureCache::sharedTextureCache()->addUIImage(image, CCString::createWithFormat("%s%s",gradientPrefix,path)->getCString());
    CC_SAFE_DELETE(image);
    return spr;
}

CCRenderTexture* ImageUtils::createStroke(CCSprite* label, float size, ccColor3B color, GLubyte opacity)
{
    
//    CCRenderTexture* rt = CCRenderTexture::create(label->getTexture()->getContentSize().width + size * 2,
//                                                  label->getTexture()->getContentSize().height  + size * 2
//                                                  );
    CCSize originSize = label->getContentSize();
    originSize = CCSizeMake(originSize.width*label->getScale(), originSize.height*label->getScale());
    
    CCRenderTexture* rt = CCRenderTexture::create(originSize.width + size * 2,originSize.height  + size * 2);
//    CCLOG("%.2f, %.2f",label->getContentSize().width,label->getContentSize().height);
    
    
    CCPoint originalPos = label->getPosition();
    
    ccColor3B originalColor = label->getColor();
    
    GLubyte originalOpacity = label->getOpacity();
    
    bool originalVisibility = label->isVisible();
    
//    float originalScale = label->getScale();
    label->setColor(color);
    
//    label->setScale(1.0);
    
    label->setOpacity(opacity);
    
    label->setVisible(true);
    
    ccBlendFunc originalBlend = label->getBlendFunc();
    
    ccBlendFunc bf = {GL_SRC_ALPHA, GL_ONE};
    
    label->setBlendFunc(bf);
    
    CCPoint bottomLeft = ccp(
                             originSize.width * label->getAnchorPoint().x + size,
                             originSize.height * label->getAnchorPoint().y + size);
    
    CCPoint positionOffset = ccp( size /2, size /2 );
    if(label->getAnchorPoint().x == 0.5f){
        positionOffset.x = size /2;
    }else if(label->getAnchorPoint().x == 0.0f){
        positionOffset.x =- originSize.width / 2;
    }else{
        positionOffset.x = size /2 + originSize.width/2;
    }
    if(label->getAnchorPoint().y == 0.5f){
        positionOffset.y = size /2;
    }else if(label->getAnchorPoint().y == 0.0f){
        positionOffset.y =- originSize.height / 2;
    }else{
        positionOffset.y =  size /2 + originSize.height/2;
    }
    
    CCPoint position = ccpSub(originalPos, positionOffset);
    
    rt->begin();
    
    for (int i=0; i<360; i+= 3) // you should optimize that for your needs
    {
        label->setPosition(
                           ccp(bottomLeft.x + sin(CC_DEGREES_TO_RADIANS(i))*size, bottomLeft.y + cos(CC_DEGREES_TO_RADIANS(i))*size)
                           );
        label->visit();
    }
    rt->end();
    
    label->setPosition(originalPos);
    label->setColor(originalColor);
    label->setBlendFunc(originalBlend);
    label->setVisible(originalVisibility);
    label->setOpacity(originalOpacity);
//    label->setScale(originalScale);
    
    rt->setPosition(position);
    rt->getSprite()->getTexture()->setAntiAliasTexParameters();
    
    return rt;
}

CCImage* ImageUtils::rgbToGray(CCTexture2D* src){
    
    CCSprite* spr=CCSprite::createWithTexture(src);
    CCRenderTexture* renderTexture=CCRenderTexture::create(spr->boundingBox().size.width, spr->boundingBox().size.height, kCCTexture2DPixelFormat_Default);
    spr->setFlipY(true);
    renderTexture->begin();
    spr->setAnchorPoint(ccp(0,0));
    spr->setPosition(ccp(0,0));
    spr->visit();
    renderTexture->end();
    
    CCImage* image=renderTexture->newCCImage();
    
    int x=3;
    if(image->hasAlpha()) {x=4;}
    unsigned char *data = image->getData();
    for(int i=0;i<image->getWidth();i++)
    {
        for(int j=0;j<image->getHeight();j++)
        {
            unsigned char *pixel = data + (i + j * image->getWidth()) * x;
            // You can see/change pixels here !
            unsigned char r = *pixel;
            unsigned char g = *(pixel + 1);
            unsigned char b = *(pixel + 2) ;
            //unsigned char a = *(pixel + 3);
            
            unsigned char gray = (0.299 * r + 0.587 * g + 0.144 * b);
            *pixel=gray;
            *(pixel+1)=gray;
            *(pixel+2)=gray;
        }
    }
    
    return image;
}

void ImageUtils::rgbToGray(CCImage* image){
    int x=3;
    if(image->hasAlpha()) {x=4;}
    unsigned char *data = image->getData();
    for(int i=0;i<image->getWidth();i++)
    {
        for(int j=0;j<image->getHeight();j++)
        {
            unsigned char *pixel = data + (i + j * image->getWidth()) * x;
            // You can see/change pixels here !
            unsigned char r = *pixel;
            unsigned char g = *(pixel + 1);
            unsigned char b = *(pixel + 2) ;
            //unsigned char a = *(pixel + 3);
            
            unsigned char gray = (0.299 * r + 0.587 * g + 0.144 * b);
            *pixel=gray;
            *(pixel+1)=gray;
            *(pixel+2)=gray;
        }
    }
}

CCImage* ImageUtils::increasedBrightness(cocos2d::CCTexture2D* src, float increasedPercentage){
    CCSprite* spr=CCSprite::createWithTexture(src);
    CCRenderTexture* renderTexture=CCRenderTexture::create(spr->boundingBox().size.width, spr->boundingBox().size.height, kCCTexture2DPixelFormat_Default);

    renderTexture->begin();
    spr->setAnchorPoint(ccp(0,0));
    spr->setPosition(ccp(0,0));
    spr->visit();
    renderTexture->end();
    
    CCImage* image=renderTexture->newCCImage();
    
    int x=3;
    if(image->hasAlpha()) {x=4;}
    unsigned char *data = image->getData();
    for(int i=0;i<image->getWidth();i++)
    {
        for(int j=0;j<image->getHeight();j++)
        {
            unsigned char *pixel = data + (i + j * image->getWidth()) * x;
            // You can see/change pixels here !
            unsigned char r = *pixel;
            unsigned char g = *(pixel + 1);
            unsigned char b = *(pixel + 2) ;
            //unsigned char a = *(pixel + 3);
            
            double fr=(double)(int)r;
            double fg=(double)(int)g;
            double fb=(double)(int)b;
/*--------------------------------------------------------------------------*/
            /*
            // HSL
            
            double cMax=MAX(fr, MAX(fg, fb));
            double cMin=MIN(fr, MIN(fg, fb));
            double delta=cMax-cMin;
            double baseH=60;
            
            double H=0;
            if(cMax==fr){
                H=baseH*fmod((fg-fb)/delta, 6);
            }else if (cMax==fg){
                H=baseH*(fb-fr)/delta+2;
            }else{
                H=baseH*(fr-fg)/delta+4;
            }
            
            double L=(cMax+cMin)/2;
            
            double S=delta/(1-fabs(2*L-1));
            
            L=L*(1+increasedPercentage*0.01)*L;
            
            double C=(1-fabs(2*L-1))*S;
            double X=C*(1-fabs(fmod(H/baseH, 2)-1));
            double m=L-C/2;
            
            if(H<baseH){
                fr=C; fg=X; fb=0;
            }else if (H<2*baseH){
                fr=X; fg=C; fb=0;
            }else if (H<3*baseH){
                fr=0; fg=C; fb=X;
            }else if (H<4*baseH){
                fr=0; fg=X; fb=C;
            }else if (H<5*baseH){
                fr=X; fg=0; fb=C;
            }else if (H<6*baseH){
                fr=C; fg=0; fb=X;
            }else; // no change to fr,fg,fb by default
            
            fr+=m; fg+=m; fb+=m;
            */
//--------------------------------------------------------------------------
            // YUV

            double y=0.299*fr+0.587*fg+0.114*fb;
            double u=(-0.14713)*fr+(-0.28886)*fg+0.436*fb;
            double v=(0.615)*fr+(-0.51499)*fg+(-0.10001)*fb;
            
            y=(1+increasedPercentage*0.01)*y;
            
            fr=y+1.13983*v;
            fg=y+(-0.39465)*u+(-0.58060)*v;
            fb=y+(2.03211)*u;

//--------------------------------------------------------------------------
            /*

            // PLAIN
             
            fr=(1+increasedPercentage*0.01)*r;
            fg=(1+increasedPercentage*0.01)*g;
            fb=(1+increasedPercentage*0.01)*b;
             
            */
            
/*--------------------------------------------------------------------------*/
            
            r=(unsigned char)(int)fr;
            g=(unsigned char)(int)fg;
            b=(unsigned char)(int)fb;
            
            *pixel=r;
            *(pixel+1)=g;
            *(pixel+2)=b;
        }
    }
    
    return image;
}

CCImage* ImageUtils::changeRGBGradient(cocos2d::CCTexture2D *src, ccColor3B colorFrom, ccColor3B colorTo){
    
    unsigned char fr=(unsigned char)colorFrom.r;
    unsigned char fg=(unsigned char)colorFrom.g;
    unsigned char fb=(unsigned char)colorFrom.b;
    
    unsigned char tr=(unsigned char)colorTo.r;
    unsigned char tg=(unsigned char)colorTo.g;
    unsigned char tb=(unsigned char)colorTo.b;
    
    
    CCSprite* spr=CCSprite::createWithTexture(src);
    CCRenderTexture* renderTexture=CCRenderTexture::create(spr->boundingBox().size.width, spr->boundingBox().size.height, kCCTexture2DPixelFormat_Default);
    spr->setFlipY(true);
    renderTexture->begin();
    spr->setAnchorPoint(ccp(0,0));
    spr->setPosition(ccp(0,0));
    spr->visit();
    renderTexture->end();
    
    CCImage* image=renderTexture->newCCImage();
    
    int x=3;
    if(image->hasAlpha()) {x=4;}
    unsigned char *data = image->getData();
    for(int i=0;i<image->getWidth();i++)
    {
        for(int j=0;j<image->getHeight();j++)
        {
            unsigned char *pixel = data + (i + j * image->getWidth()) * x;
            float ratio=(float)i/image->getWidth();
            int r = (int)fr+(int)(tr-fr)*ratio;
            int g = (int)fg+(int)(tg-fg)*ratio;
            int b = (int)fb+(int)(tb-fb)*ratio;
            
            *pixel=(unsigned char)r;
            *(pixel+1)=(unsigned char)g;
            *(pixel+2)=(unsigned char)b;
        }
    }
    
    return image;
}

CCRect ImageUtils::calculateRect(CCSprite* spr, CCRect rect) {
    // calculate rect
    spr->setAnchorPoint(ccp(0,0));
#if kDebugLoading == 1
    CCLog( "gid %ld - (%f, %f)", gid, spr->boundingBox().size.width, spr->boundingBox().size.height);
#endif
    CCRenderTexture* renderTexture=CCRenderTexture::create(spr->boundingBox().size.width, spr->boundingBox().size.height, kCCTexture2DPixelFormat_RGBA8888);
    spr->setFlipY(true);
    renderTexture->begin();
    spr->visit();
    renderTexture->end();
    
    int offsetX = 0;
    int offsetY = 0;
    int offsetY2 = 0;
    CCImage* image=renderTexture->newCCImage();
    do {
        int width = image->getWidth();
        int height = image->getHeight();
#if kDebugLoading == 1
        CCLog( "gid %ld - image (%d, %d)", gid, width, height);
#endif
        CC_BREAK_IF(!image->hasAlpha());
        int depth=4;
        unsigned char *data = image->getData();
        
        offsetX = 0;
        for(int i=offsetX;i<width;i++)
        {
            bool hasColor = false;
            for(int j=0;j<height;j++)
            {
                unsigned char *pixel = data + (i + j * width) * depth;
                unsigned char a = *(pixel + 3);
                if ( a == 0xff ) {
                    hasColor = true;
                    break;
                }
            }
            if ( !hasColor ) {
                offsetX++;
            } else {
                break;
            }
        }
        
        offsetY = 0;
        for(int j=offsetY;j<height;j++)
        {
            bool hasColor = false;
            for(int i=0;i<width;i++)
            {
                unsigned char *pixel = data + (i + j * width) * depth;
                // You can see/change pixels here !
                unsigned char a = *(pixel + 3);
                if ( a == 0xff ) {
                    hasColor = true;
                    break;
                }
                //                CCLog( "(%d,%d) - (%d,%d,%d)", i, j, r, g, b );
            }
            if ( !hasColor ) {
                offsetY++;
            } else {
                break;
            }
        }
        
        offsetY2 = 0;
        for(int j = height-1 - offsetY2; j >= 0;j--)
        {
            bool hasColor = false;
            for(int i=0;i<width;i++)
            {
                unsigned char *pixel = data + (i + j * width) * depth;
                // You can see/change pixels here !
                unsigned char a = *(pixel + 3);
                if ( a == 0xff ) {
                    hasColor = true;
                    break;
                }
                //                CCLog( "(%d,%d) - (%d,%d,%d)", i, j, r, g, b );
            }
            if ( !hasColor ) {
                offsetY2++;
            } else {
                break;
            }
        }
    } while ( 0 );
    
    CC_SAFE_DELETE(image);
    spr->setFlipY(false );
    
    //        CCLog( "Offset (%d,%d)", offsetX, offsetY );
    CCRect output = CCRectMake(0, 0, 0, 0);
    output.origin.x = offsetX;
    output.origin.y = offsetY;
    output.size.width = rect.size.width * CC_CONTENT_SCALE_FACTOR() - offsetX - 1;
    output.size.height = rect.size.height * CC_CONTENT_SCALE_FACTOR() - offsetY - offsetY2;
    output = CC_RECT_PIXELS_TO_POINTS(output);
    
    return output;
}

CCPoint ImageUtils::getPositionByFrameworkRatio(CCNode* parent, float xRatio, float yRatio){
    if(parent==NULL){
        return CCPointZero;
    }
    return ccp(parent->getContentSize().width*(1+xRatio)*0.5, parent->getContentSize().height*(1+yRatio)*0.5);
}

void ImageUtils::addChildByFrameworkRatio(CCNode* child, CCNode* parent, float xRatio, float yRatio){
    if(child==NULL||parent==NULL){
        return;
    }
    CCPoint position=getPositionByFrameworkRatio(parent, xRatio, yRatio);
    child->setPosition(position);
    parent->addChild(child);
}


void ImageUtils::dimScreen( CCLayer * layer )
{
    CCSize wins = CCDirector::sharedDirector()->getWinSize();
    CCSprite *dim =  CCSprite::create("blackpixel.png");
    dim->setScaleX(wins.width);
    dim->setScaleY(wins.height);
    dim->setAnchorPoint(ccp(0,0));
    dim->setPosition(ccp(0,0));
    dim->setOpacity( floor(256 * .80) );                // make the opacity 70%
    layer->addChild(dim, 0, kTagDimBackground);
}

void ImageUtils::undimScreen( CCLayer * layer )
{
    layer->removeChildByTag(kTagDimBackground);
}

CCAction* ImageUtils::toggleScaling(float firstDuration, float firstScale, float secondDuration, float secondScale){
    CCScaleTo* scaleUp=CCScaleTo::create(firstDuration, firstScale);
    CCScaleTo* scaleDown=CCScaleTo::create(secondDuration, secondScale);
    CCSequence* actions=CCSequence::create(scaleUp, scaleDown, NULL);
    return actions;
}