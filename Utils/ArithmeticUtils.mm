#include <stdio.h>
#include "ArithmeticUtils.h"

static void getTimeZone(){
}

int GWUtilities::getTimeZoneOffset(){
    NSDate *date = [NSDate date];
    NSDateFormatter *dateFormat = [[NSDateFormatter alloc] init];
    [dateFormat setDateFormat:@"ZZZ"];
    NSString *zoneOffset = [dateFormat stringFromDate:date];
    unichar signChar = [zoneOffset characterAtIndex:0];
    int sign = (signChar == '+') ? +1 : -1;
    int hour1 = [zoneOffset characterAtIndex:1] - '0';
    int hour2 = [zoneOffset characterAtIndex:2] - '0';
    
    int hour = hour1 * 10 + hour2;
    
    int min1 = [zoneOffset characterAtIndex:3] - '0';
    int min2 = [zoneOffset characterAtIndex:4] - '0';
    int min = min1 *10 + min2;
    return sign* hour* 3600 + min * 60;
}
