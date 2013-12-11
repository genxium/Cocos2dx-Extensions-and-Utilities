#include "ArithmeticUtils.h"
#include "cocos2d.h"
#include "ImageUtils.h"
#include <sys/types.h>
#include <sys/sysctl.h>

CCString* ArithmeticUtils::getTimeInStr( float time ) {
    time += 0.5;
    if ( time >= (60 * 60 * 24) ) {
        // greater than 1 day
        float hours = floorf( fmodf(time / 3600, 24));
        int days = time/(60 * 60 * 24);
        if ( hours == 0 ) {
            return CCString::createWithFormat( "%dd", days );
        }
        return CCString::createWithFormat( "%.dd %.0fh", days, hours );
    } else if ( time >= (60 * 60) ) {
        // greater than 60 mins
        float minutes = floorf( fmodf(time / 60.0, 60));
        int hours = (time / 3600);
        if ( minutes == 0) {
            return CCString::createWithFormat( "%dh", hours);
        } 
        return CCString::createWithFormat( "%dh %.0fm", hours ,minutes );
    } else {
        float seconds = floorf( fmodf(time, 60) );
        if ( time > 59.5 ) {
            int minutes = (time / 60);
            if ( seconds == 0 ) {
                return CCString::createWithFormat( "%dm", minutes );
            }
            return CCString::createWithFormat( "%dm %.0fs",minutes ,seconds );
        }else {
            if ( seconds == 0 ) {
                return CCString::createWithFormat( "1s" );
            }
            return CCString::createWithFormat( "%.0fs",seconds );
        }
    }
}

CCString* ArithmeticUtils::getTimeInStr2( float time ) {
    int firstUnit;
    int secondUnit;
    std::string firstUnitStr = "";
    std::string secondUnitStr = "";
    if ( time > 2 * 60 * 60 * 24 ) {
        // greater than 1 day
        firstUnit = time/(60 * 60 * 24);
        secondUnit = fmodf(time / 3600, 24);
        
        if ( secondUnit >= 30 ) {
            firstUnit++;
        }
        secondUnit = 0;
        
        firstUnitStr = "day";
        if ( firstUnit > 1) {
            firstUnitStr= "days";
        }
//        secondUnitStr = "hour";
//        if ( secondUnit > 1) {
//            secondUnitStr= "hours";
//        }
    } else if ( time > 60 * 60 * 4 ) {
        // greater than 4 hours
        firstUnit = (time / 3600);
        secondUnit = fmodf(time / 60, 60);
        if ( secondUnit >= 30 ) {
            firstUnit++;
        }
        secondUnit = 0;
        
        firstUnitStr = "hour";
        if ( firstUnit > 1) {
            firstUnitStr= "hours";
        }
    } else if ( time > 240 * 60 ) {
        // greater than 240 mins ( 2 hour)
        firstUnit = (time / 3600); // hours 
        secondUnit = fmodf(time / 60, 60); // mins
        
        firstUnitStr = "hour";
        if ( firstUnit > 1) {
            firstUnitStr= "hours";
        }
        secondUnitStr = "min";
        if ( secondUnit > 1) {
            secondUnitStr= "mins";
        }
    } else {
        // less then than 60 mins ( 1 hour)
        firstUnit = (time / 60); // minutes
        secondUnit = fmodf(time, 60); // seconds
        if ( secondUnit >= 30 ) {
            firstUnit++;
        }
        secondUnit = 0;
        
        firstUnitStr = "min";
        if ( firstUnit > 1) {
            firstUnitStr= "mins";
        }
    }
    if ( secondUnit == 0 ) {
        return CCString::createWithFormat( "%d%s", firstUnit, firstUnitStr.c_str() );
    } else  {
        return CCString::createWithFormat( "%d%s %d%s", firstUnit, firstUnitStr.c_str(), secondUnit, secondUnitStr.c_str() );
    }
}

CCString* ArithmeticUtils::getCollectResourcesInStr( float value ) {
    CCString * price = CCString::createWithFormat( "+%.0f", value);
    return price;
}

int ArithmeticUtils::wordCount(const string &str, int& maxWordLength){
    int ret=0;
    for(int i=0;i<str.size();++i){
        if(str[i]!=' '){
            ++ret; int wordLength=1;
            while(i+1<str.size() && str[++i]!=' ') {++wordLength;};
            maxWordLength=maxWordLength>wordLength?maxWordLength:wordLength;
        }
        else{
            while(i+1<str.size() && str[++i]==' ');
        }
    }
    return ret;
}

int ArithmeticUtils::characterCount(const std::string &str){
    return str.size();
}

int ArithmeticUtils::getFontSizeByValueAndBasicSize(float value, int basicSize){
    int numberOfDigits= fabs(value)>0?(int)(ceil(log(fabs(value))/log(10))+1):1;
    return basicSize-(numberOfDigits)+1;
}

float ArithmeticUtils::getFontScaleFactorByValueAndBasicScale(float value, float basicScale){
    int numberOfDigits= fabs(value)>0?(int)(ceil(log(fabs(value))/log(10))+1):1;
    return basicScale-0.02*(float)numberOfDigits;
}

CCString * ArithmeticUtils::getMachineName() {
    size_t size;
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);
    char *name = (char *) malloc(size);
    sysctlbyname("hw.machine", name, &size, NULL, 0);
    CCString* string = CCString::create(name);
    free(name);
    return string;
}

CCString* ArithmeticUtils::getRankStr(int rank){
    int remainder=rank%10;
    CCString* res=NULL;
    switch (remainder) {
        case 1:
            res=CCString::createWithFormat("%dst", rank);
            break;
        case 2:
            res=CCString::createWithFormat("%dnd", rank);
            break;
        case 3:
            res=CCString::createWithFormat("%drd", rank);
            break;
        default:
            res=CCString::createWithFormat("%dth", rank);
            break;
    }
    return res;
}

CCString* ArithmeticUtils::getRewardRateStr(float rewardValue, float rewardCycleInSec){
    CCString* res=NULL;
    float secsPerMin=60.0, minsPerHour=60, hoursPerDay=24;
    float valueInMin=rewardValue*secsPerMin/rewardCycleInSec;
    float valueInHour=rewardValue*secsPerMin*minsPerHour/rewardCycleInSec;
    float valueInDay=rewardValue*secsPerMin*minsPerHour*hoursPerDay/rewardCycleInSec;
    if(valueInMin>=1.0){
        res=CCString::createWithFormat("%.0f/MIN", valueInMin);
    } else if(valueInHour>=1.0){
        res=CCString::createWithFormat("%.0f/HOUR", valueInHour);
    } else if(valueInDay>=1.0){
        res=CCString::createWithFormat("%.0f/DAY", valueInDay);
    } else{
        res=CCString::createWithFormat("");
    }
    return res;
}
