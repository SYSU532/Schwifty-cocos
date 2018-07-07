#pragma once
#ifndef __PICK__PAGE__H
#define __PICK__PAGE__H

#include "cocos2d.h"
#include <vector>

using namespace cocos2d;
using namespace std;

class PickPage : public Scene {
public:
	static Scene* createScene();

	virtual bool init();
	bool onTouchBegan(Touch* t, Event* e);

	void onSubmit(Ref* pSender);
	void onStart(Ref* pSender);

	CREATE_FUNC(PickPage);

private:
	Vector<Sprite*> rickArr;
	vector<string> rickName;

	Label* choice;

	std::string username;
	int whichRick;

};

#endif