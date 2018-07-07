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
	void initUserInfo();

	// Touch Functions
	void onTouchMoved(Touch* touch, Event* event);
	bool onTouchBegin(Touch* touch, Event* event);

	// KeyBoard Functions
	void onKeyPress(EventKeyboard::KeyCode code, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode code, Event* event);

	// Mouse Functions
	void onMouseMoved(Event* event);

	// Event Listeners
	void addTouchListener();
	void addMouseListener();
	void addKeyBoardListener();

	// Music Handler
	void preLoadMusic();
	void playBgm();
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

private:
	Size visibleSize;
	Vec2 origin;
	std::string username;
	std::string path;
	int userLevel;
	int userCardsNum;
	int userHeadImgType;
	bool  outFlag;

	cocos2d::Camera * cam;
	Sprite* player;
	Sprite* book;
	char movekey;
	bool isMove;
	int state;

};

#endif // __HELLOWORLD_SCENE_H__
