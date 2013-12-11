//
//  CCHttpClientExMultiThreading.h
//  ScrollTest
//
//  Created by Wing on 8/10/13.
//
//

#ifndef __ScrollTest__CCHttpClientExMultiThreading__
#define __ScrollTest__CCHttpClientExMultiThreading__

#include <iostream>
#include "cocos2d.h"
#include "ExtensionMacros.h"

#include "HttpRequest.h"
#include "HttpResponse.h"

USING_NS_CC;
USING_NS_CC_EXT;

class CCHttpClientExMultiThreading: public CCObject
{
public:
    /** Return the shared instance **/
    static CCHttpClientExMultiThreading *getInstance();
    
    /** Relase the shared instance **/
    static void destroyInstance();
    
    /**
     * Add a get request to task queue
     * @param request a CCHttpRequest object, which includes url, response callback etc.
     please make sure request->_requestData is clear before calling "send" here.
     * @return NULL
     */
    void send(CCHttpRequest* request);
    
    /**
     * Change the connect timeout
     * @param timeout
     * @return NULL
     */
    inline void setTimeoutForConnect(int value) {_timeoutForConnect = value;};
    
    /**
     * Get connect timeout
     * @return int
     *
     */
    inline int getTimeoutForConnect() {return _timeoutForConnect;}
    
    /**
     * Change the download timeout
     * @param value
     * @return NULL
     */
    inline void setTimeoutForRead(int value) {_timeoutForRead = value;};
    
    
    /**
     * Get download timeout
     * @return int
     */
    inline int getTimeoutForRead() {return _timeoutForRead;};
    
    void haltNetworkThread();
    bool isHalted();
    
private:
    CCHttpClientExMultiThreading();
    virtual ~CCHttpClientExMultiThreading();
    bool init(void);
    
    /**
     * Init pthread mutex, semaphore, and create new thread for http requests
     * @return bool
     */
    bool lazyInitThreadSemphore();
    /** Poll function called from main thread to dispatch callbacks when http requests finished **/
    void dispatchResponseCallbacks(float delta);
    
private:
    int _timeoutForConnect;
    int _timeoutForRead;
    
};

#endif /* defined(__ScrollTest__CCHttpClientExMultiThreadingMultiThreading__) */
