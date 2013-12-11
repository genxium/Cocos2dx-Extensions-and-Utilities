#ifndef __ArithmeticUtils__
#define __ArithmeticUtils__

#include "cocos2d.h"
USING_NS_CC;

class ArithmeticUtils : public CCObject {
public:
    static CCString* getTimeInStr( float time );
    static CCString* getTimeInStr2( float time );
    static CCString* getCollectResourcesInStr( float value );
    static int wordCount(const std::string& , int& maxWordLength);
    static int characterCount(const std::string&);
    static int getFontSizeByValueAndBasicSize(float value, int basicSize);
    static float getFontScaleFactorByValueAndBasicScale(float value, float basicScale);
    static CCString * getMachineName();
    static int getTimeZoneOffset();
    static CCString* getRankStr(int rank);
    static CCString* getRewardRateStr(float rewardValue, float rewardCycleInSec);
};