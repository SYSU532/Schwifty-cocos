#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

using namespace cocos2d;
class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

	void placeTileMap();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

	void menuClickCallback(cocos2d::Ref* pSender);

	// Basic Role Behaviors
	void attack();
	void move(char c);

	void update(float f);

	// Touch Functions
	void onTouchMoved(Touch* touch, Event* event);
	bool onTouchBegin(Touch* touch, Event* event);

	// KeyBoard Functions
	void onKeyPress(EventKeyboard::KeyCode code, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode code, Event* event);

	// Event Listeners
	void addTouchListener();
	void addKeyBoardListener();
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

private:
	Size visibleSize;
	Vec2 origin;

	cocos2d::Camera * cam;
	Sprite* player;
	char movekey;
	bool isMove;

};

#endif // __HELLOWORLD_SCENE_H__
