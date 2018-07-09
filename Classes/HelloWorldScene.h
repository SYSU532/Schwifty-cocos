#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "MyDialog.h"
#include <vector>

using namespace cocos2d;
using namespace std;

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

	void placeTileMap();
    
	// Basic Role Behaviors
	void sendGameRequest();
	void showRequestIcon();
	void sendNetWorkRequest(Node* node);
	void jumpToPlayCards(Node* node);
	void acceptInvitation(Node* node);

	void move(char c);

	void update(float f);
	void networkUpdate(float f);

	void initUserInfo();
	void initAllUsers();

	void addNewUser(string username, int rickType, Vec2 initLoc);
	void deleteUser(string username);
	bool checkUserIsIn(string username);

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
	// Window Basic Properties
	Size visibleSize;
	Vec2 origin;

	// User Infos
	std::string username;
	std::string path;
	int userCardsNum;
	string userKey;
	int userRick;
	char movekey;
	bool isMove;
	int state;

	// Scene elements
	bool outFlag;
	Sprite* player;
	vector<pair<string, Sprite*> > otherPlayers;
	vector<pair<string, bool> > gameRequests;
	Sprite* book;
	string targetUserName;
	string requestUserName;
	
	// WebSocket Messages register
	string msgType;
	string nowMsg;
	vector<string> splitMsg;

};

#endif 
