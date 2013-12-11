//
//  CCHttpClientEx.cpp
//  Puzzle
//
//  Created by Wing on 16/9/13.
//
//

#include "CCHttpClientEx.h"
#include <queue>
#include <pthread.h>
#include <errno.h>
#include "curl/curl.h"
#include "DownloaderDef.h"

static pthread_t        s_networkThread;
static pthread_mutex_t  s_requestQueueMutex;
static pthread_mutex_t  s_responseQueueMutex;

static pthread_mutex_t		s_SleepMutex;
static pthread_cond_t		s_SleepCondition;

static unsigned long    s_asyncRequestCount = 0;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
typedef int int32_t;
#endif

static bool need_quit = false;
static bool bIsHalted = false;
static pthread_mutex_t s_bHaltMutex=PTHREAD_MUTEX_INITIALIZER;

static CCArray* s_requestQueue = NULL;
static CCArray* s_responseQueue = NULL;

static CCHttpClientEx *s_pHttpClient = NULL; // pointer to singleton

static char s_errorBuffer[CURL_ERROR_SIZE];

typedef size_t (*write_callback)(void *ptr, size_t size, size_t nmemb, void *stream);

// Callback function used by libcurl for collect response data
static size_t writeData(void *ptr, size_t size, size_t nmemb, void *stream)
{
#if kUseBruteInterrupt == 1
    bool haltSignal=false;
#if kUseTryLock == 1
    if(0==pthread_mutex_trylock(&s_bHaltMutex)){
#else
    if(0==pthread_mutex_lock(&s_bHaltMutex)){
#endif
        haltSignal=bIsHalted;
        pthread_mutex_unlock(&s_bHaltMutex);
    }
    
    if(true==haltSignal){
        return CURLE_WRITE_ERROR;
    }
#endif
        
    std::vector<char> *recvBuffer = (std::vector<char>*)stream;
    size_t sizes = size * nmemb;
    
    // add data to the end of recvBuffer
    // write data maybe called more than once in a single request
    recvBuffer->insert(recvBuffer->end(), (char*)ptr, (char*)ptr+sizes);
    
    return sizes;
}

// Prototypes
static bool configureCURL(CURL *handle);
static int processGetTask(CCHttpRequest *request, write_callback callback, void *stream, int32_t *errorCode);
static int processPostTask(CCHttpRequest *request, write_callback callback, void *stream, int32_t *errorCode);

// Worker thread
static void* networkThread(void *data)
{
    CCHttpRequest *request = NULL;
    
    while (true)
    {
        if (need_quit)
        {
            break;
        }

        bool haltSignal=false;
#if kUseTryLock == 1
        if(0==pthread_mutex_trylock(&s_bHaltMutex)){
#else
        if(0==pthread_mutex_lock(&s_bHaltMutex)){
#endif
            haltSignal=bIsHalted;
            pthread_mutex_unlock(&s_bHaltMutex);
        }
        
        if(true==haltSignal){
            // Wait for http request tasks from main thread
        	pthread_cond_wait(&s_SleepCondition, &s_SleepMutex);
            continue;
        }
    
        // step 1: send http request if the requestQueue isn't empty
        request = NULL;
        
        pthread_mutex_lock(&s_requestQueueMutex); //Get request task from queue
        if (s_requestQueue->count()>0)
        {
            // stack operation
#if kUseRequestStack == 1
            request = dynamic_cast<CCHttpRequest*>(s_requestQueue->lastObject());
            s_requestQueue->removeLastObject();
#else
            request = dynamic_cast<CCHttpRequest*>(s_requestQueue->objectAtIndex(0));
            s_requestQueue->removeObjectAtIndex(0);
#endif
            // request's refcount = 1 here
        }
        pthread_mutex_unlock(&s_requestQueueMutex);
        
        if (NULL == request) // suspend the thread on requestQueue empty or intentionally halted
        {
        	// Wait for http request tasks from main thread
        	pthread_cond_wait(&s_SleepCondition, &s_SleepMutex);
            continue;
        }
        
        // step 2: libcurl sync access
        
        // Create a HttpResponse object, the default setting is http access failed
        CCHttpResponse *response = new CCHttpResponse(request);
        
        // request's refcount = 2 here, it's retained by HttpRespose constructor
        request->release();
        // ok, refcount = 1 now, only HttpResponse hold it.
        
        int responseCode = -1;
        int retValue = 0;
        
        // Process the request -> get response packet
        switch (request->getRequestType())
        {
            case CCHttpRequest::kHttpGet: // HTTP GET
                retValue = processGetTask(request,
                                          writeData,
                                          response->getResponseData(),
                                          &responseCode);
                break;
                
            case CCHttpRequest::kHttpPost: // HTTP POST
                retValue = processPostTask(request,
                                           writeData,
                                           response->getResponseData(),
                                           &responseCode);
                break;
                
            default:
                CCAssert(true, "CCHttpClientEx: unkown request type, only GET and POSt are supported");
                break;
        }
        
        // write data to HttpResponse
        response->setResponseCode(responseCode);
        
        if (retValue != 0)
        {
            response->setSucceed(false);
            response->setErrorBuffer(s_errorBuffer);
        }
        else
        {
            response->setSucceed(true);
        }
        
        // add response packet into queue
        pthread_mutex_lock(&s_responseQueueMutex);
        s_responseQueue->addObject(response);
        pthread_mutex_unlock(&s_responseQueueMutex);
        
        // resume dispatcher selector
        CCDirector::sharedDirector()->getScheduler()->resumeTarget(CCHttpClientEx::getInstance());
    }
    
    // cleanup: if worker thread received quit signal, clean up un-completed request queue
    pthread_mutex_lock(&s_requestQueueMutex);
    s_requestQueue->removeAllObjects();
    pthread_mutex_unlock(&s_requestQueueMutex);
    s_asyncRequestCount -= s_requestQueue->count();
    
    if (s_requestQueue != NULL) {
        
        pthread_mutex_destroy(&s_requestQueueMutex);
        pthread_mutex_destroy(&s_responseQueueMutex);
        
        pthread_mutex_destroy(&s_SleepMutex);
        pthread_cond_destroy(&s_SleepCondition);
        
        s_requestQueue->release();
        s_requestQueue = NULL;
        s_responseQueue->release();
        s_responseQueue = NULL;
    }
    
    pthread_exit(NULL);
    
    return 0;
}

//Configure curl's timeout property
bool configureCURL(CURL *handle)
{
    if (!handle) {
        return false;
    }
    
    int32_t code;
    code = curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, s_errorBuffer);
    if (code != CURLE_OK) {
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_TIMEOUT, CCHttpClientEx::getInstance()->getTimeoutForRead());
    if (code != CURLE_OK) {
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, CCHttpClientEx::getInstance()->getTimeoutForConnect());
    if (code != CURLE_OK) {
        return false;
    }
    
    return true;
}

//Process Get Request
int processGetTask(CCHttpRequest *request, write_callback callback, void *stream, int *responseCode)
{
    const char* url = request->getUrl();
    std::string urlStr = url;
    
    CURLcode code = CURL_LAST;
    CURL *curl = curl_easy_init();
    
    do {
        if (!configureCURL(curl))
        {
            break;
        }
        
        /* handle custom header data */
        /* create curl linked list */
        struct curl_slist *cHeaders=NULL;
        /* get custom header data (if set) */
       	std::vector<std::string> headers=request->getHeaders();
        if(!headers.empty())
        {
            for(std::vector<std::string>::iterator it=headers.begin();it!=headers.end();it++)
            {
                /* append custom headers one by one */
                cHeaders=curl_slist_append(cHeaders,it->c_str());
            }
            /* set custom headers for curl */
            code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, cHeaders);
            if (code != CURLE_OK) {
                break;
            }
        }
        
        code = curl_easy_setopt(curl, CURLOPT_URL, request->getUrl());
        if (code != CURLE_OK)
        {
            break;
        }
        
        code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, request->getUrl());
        if (code != CURLE_OK)
        {
            break;
        }
        
        code = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        if (code != CURLE_OK)
        {
            break;
        }
        
        code = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
        if (code != CURLE_OK)
        {
            break;
        }
        
        code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        if (code != CURLE_OK)
        {
            break;
        }
        
        code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, stream);
        if (code != CURLE_OK)
        {
            break;
        }
        
        code = curl_easy_perform(curl);
        if (code != CURLE_OK)
        {
            break;
        }
        
        /* free the linked list for header data */
        curl_slist_free_all(cHeaders);
        
        code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, responseCode);
        if (code != CURLE_OK || *responseCode != 200)
        {
            code = CURLE_HTTP_RETURNED_ERROR;
        }
    } while (0);
    
    if (curl) {
        curl_easy_cleanup(curl);
    }
    
    return (code == CURLE_OK ? 0 : 1);
}

//Process POST Request
int processPostTask(CCHttpRequest *request, write_callback callback, void *stream, int32_t *responseCode)
{
    CURLcode code = CURL_LAST;
    CURL *curl = curl_easy_init();
    
    do {
        if (!configureCURL(curl)) {
            break;
        }
        
        /* handle custom header data */
        /* create curl linked list */
        struct curl_slist *cHeaders=NULL;
        /* get custom header data (if set) */
        std::vector<std::string> headers=request->getHeaders();
        if(!headers.empty())
        {
            for(std::vector<std::string>::iterator it=headers.begin();it!=headers.end();it++)
            {
                /* append custom headers one by one */
                cHeaders=curl_slist_append(cHeaders,it->c_str());
            }
            /* set custom headers for curl */
            code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, cHeaders);
            if (code != CURLE_OK) {
                break;
            }
        }
        
        code = curl_easy_setopt(curl, CURLOPT_URL, request->getUrl());
        if (code != CURLE_OK) {
            break;
        }
        code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        if (code != CURLE_OK) {
            break;
        }
        code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, stream);
        if (code != CURLE_OK) {
            break;
        }
        code = curl_easy_setopt(curl, CURLOPT_POST, 1);
        if (code != CURLE_OK) {
            break;
        }
        code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request->getRequestData());
        if (code != CURLE_OK) {
            break;
        }
        code = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request->getRequestDataSize());
        if (code != CURLE_OK) {
            break;
        }
        code = curl_easy_perform(curl);
        if (code != CURLE_OK) {
            break;
        }
        
        /* free the linked list for header data */
        curl_slist_free_all(cHeaders);
        
        code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, responseCode);
        if (code != CURLE_OK || *responseCode != 200) {
            code = CURLE_HTTP_RETURNED_ERROR;
        }
    } while (0);
    if (curl) {
        curl_easy_cleanup(curl);
    }
    
    return (code == CURLE_OK ? 0 : 1);
}

// HttpClient implementation
CCHttpClientEx* CCHttpClientEx::getInstance()
{
    if (s_pHttpClient == NULL) {
        s_pHttpClient = new CCHttpClientEx();
    }
    
    return s_pHttpClient;
}

void CCHttpClientEx::destroyInstance()
{
    CCAssert(s_pHttpClient, "");
    CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(CCHttpClientEx::dispatchResponseCallbacks), s_pHttpClient);
    s_pHttpClient->release();
}

CCHttpClientEx::CCHttpClientEx()
: _timeoutForConnect(30)
, _timeoutForRead(60)
{
    CCDirector::sharedDirector()->getScheduler()->scheduleSelector(
                                                                   schedule_selector(CCHttpClientEx::dispatchResponseCallbacks), this, 0, false);
    CCDirector::sharedDirector()->getScheduler()->pauseTarget(this);
}

CCHttpClientEx::~CCHttpClientEx()
{
    need_quit = true;
    
    if (s_requestQueue != NULL) {
    	pthread_cond_signal(&s_SleepCondition);
    }
    
    s_pHttpClient = NULL;
}

//Lazy create semaphore & mutex & thread
bool CCHttpClientEx::lazyInitThreadSemphore()
{
#if kUseTryLock == 1
    if(0==pthread_mutex_trylock(&s_bHaltMutex)){
#else
    if(0==pthread_mutex_lock(&s_bHaltMutex)){
#endif
        bIsHalted = false;
        pthread_mutex_unlock(&s_bHaltMutex);
    }
    
    if (s_requestQueue != NULL) {
        return true;
    } else {
        
        s_requestQueue = new CCArray();
        s_requestQueue->init();
        
        s_responseQueue = new CCArray();
        s_responseQueue->init();
        
        pthread_mutex_init(&s_requestQueueMutex, NULL);
        pthread_mutex_init(&s_responseQueueMutex, NULL);
        
        pthread_mutex_init(&s_SleepMutex, NULL);
        pthread_cond_init(&s_SleepCondition, NULL);
        
        pthread_create(&s_networkThread, NULL, networkThread, NULL);
        pthread_detach(s_networkThread);
        
        need_quit = false;
    }
    
    return true;
}

//Add a get task to queue
void CCHttpClientEx::send(CCHttpRequest* request)
{
    if (false == lazyInitThreadSemphore())
    {
        return;
    }
    
    if (!request)
    {
        return;
    }
    
    ++s_asyncRequestCount;
    
    request->retain();
    
    pthread_mutex_lock(&s_requestQueueMutex);
    s_requestQueue->addObject(request);
    pthread_mutex_unlock(&s_requestQueueMutex);
    
    // Notify thread start to work
    pthread_cond_signal(&s_SleepCondition);
}

// Poll and notify main thread if responses exists in queue
void CCHttpClientEx::dispatchResponseCallbacks(float delta)
{
    CCHttpResponse* response = NULL;
    
    pthread_mutex_lock(&s_responseQueueMutex);
    if (s_responseQueue->count())
    {
#if kUseResponseStack == 1
        response = dynamic_cast<CCHttpResponse*>(s_responseQueue->lastObject());
        s_responseQueue->removeLastObject();
#else
        response = dynamic_cast<CCHttpResponse*>(s_responseQueue->objectAtIndex(0));
        s_responseQueue->removeObjectAtIndex(0);
#endif
    }
    pthread_mutex_unlock(&s_responseQueueMutex);
    
    if (response)
    {
        --s_asyncRequestCount;
        
        CCHttpRequest *request = response->getHttpRequest();
        CCObject *pTarget = request->getTarget();
        SEL_CallFuncND pSelector = request->getSelector();
        
        if (pTarget && pSelector)
        {
            (pTarget->*pSelector)((CCNode *)this, response);
        }
        
        response->release();
    }
    
    if (0 == s_asyncRequestCount)
    {
        CCDirector::sharedDirector()->getScheduler()->pauseTarget(this);
    }
    
}

void CCHttpClientEx::haltNetworkThread(){
#if kUseTryLock == 1
    if(0==pthread_mutex_trylock(&s_bHaltMutex)){
#else
    if(0==pthread_mutex_lock(&s_bHaltMutex)){
#endif
        bIsHalted=true;
        pthread_mutex_unlock(&s_bHaltMutex);
    }
}

bool CCHttpClientEx::isHalted(){
    bool ret=false;
#if kUseTryLock == 1
    if(0==pthread_mutex_trylock(&s_bHaltMutex)){
#else
    if(0==pthread_mutex_lock(&s_bHaltMutex)){
#endif
        ret=bIsHalted;
        pthread_mutex_unlock(&s_bHaltMutex);
    }
    return ret;
}
