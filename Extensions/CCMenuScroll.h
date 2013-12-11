//
//  CCMenuScroll.h
//  Puzzle
//
//  Created by Tact Sky on 15/8/13.
//
//

#ifndef __Puzzle__CCMenuScroll__
#define __Puzzle__CCMenuScroll__

#include <iostream>
#include <vector>
#include "cocos2d.h"
using namespace std;
USING_NS_CC;

#define kHighestPriority INT_MIN

class CCMenuScroll: public CCMenu
{
    /** whether or not the menu will receive events */
    bool m_bEnabled;
    
public:
    CCMenuScroll();
    virtual ~CCMenuScroll();
    
    bool isDragging();
    
    /** creates an empty CCMenu */
    static CCMenuScroll* create();
    
    /** creates a CCMenu with CCMenuItem objects */
    static CCMenuScroll* create(CCMenuItem* item, ...);
    
    /** creates a CCMenu with a CCArray of CCMenuItem objects */
    static CCMenuScroll* createWithArray(CCArray* pArrayOfItems);
    
    /** creates a CCMenu with it's item, then use addChild() to add
     * other items. It is used for script, it can't init with undetermined
     * number of variables.
     */
    static CCMenuScroll* createWithItem(CCMenuItem* item);
    
    /** creates a CCMenu with CCMenuItem objects */
    static CCMenuScroll* createWithItems(CCMenuItem *firstItem, va_list args);
    
    /** initializes an empty CCMenu */
    bool init();
    
    /** initializes a CCMenu with a NSArray of CCMenuItem objects */
    bool initWithArray(CCArray* pArrayOfItems);
    
    /** align items vertically */
    void alignItemsVertically();
    /** align items vertically with padding
     @since v0.7.2
     */
    void alignItemsVerticallyWithPadding(float padding);
    
    /** align items horizontally */
    void alignItemsHorizontally();
    /** align items horizontally with padding
     @since v0.7.2
     */
    void alignItemsHorizontallyWithPadding(float padding);
    
    /** align items in rows of columns */
    void alignItemsInColumns(unsigned int columns, ...);
    void alignItemsInColumns(unsigned int columns, va_list args);
    void alignItemsInColumnsWithArray(CCArray* rows);
    
    /** align items in columns of rows */
    void alignItemsInRows(unsigned int rows, ...);
    void alignItemsInRows(unsigned int rows, va_list args);
    void alignItemsInRowsWithArray(CCArray* columns);
    
    /** set event handler priority. By default it is: kCCMenuTouchPriority */
    void setHandlerPriority(int newPriority);
    
    //super methods
    virtual void addChild(CCNode * child);
    virtual void addChild(CCNode * child, int zOrder);
    virtual void addChild(CCNode * child, int zOrder, int tag);
    virtual void registerWithTouchDispatcher();
    virtual void removeChild(CCNode* child, bool cleanup);
    
    /**
     @brief For phone event handle functions
     */
    virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch* touch, CCEvent* event);
    virtual void ccTouchCancelled(CCTouch *touch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch* touch, CCEvent* event);
    
    /**
     @since v0.99.5
     override onExit
     */
    virtual void onExit();
    
    virtual void setOpacityModifyRGB(bool bValue) {CC_UNUSED_PARAM(bValue);}
    virtual bool isOpacityModifyRGB(void) { return false;}
    
    virtual bool isEnabled() { return m_bEnabled; }
    virtual void setEnabled(bool value) { m_bEnabled = value; };
    
    virtual void setSwallowTouch(bool value);
    
protected:
    CCMenuItem* itemForTouch(CCTouch * touch);
    tCCMenuState m_eState;
    CCMenuItem *m_pSelectedItem;
    
private:
    CCPoint _lastTouchBegan, _lastTouchEnded;
    float _stillTouchThreshold;
};

#endif /* defined(__Puzzle__CCMenuScroll__) */
