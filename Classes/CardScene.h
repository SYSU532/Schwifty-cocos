#pragma once
#ifndef __CARD__SCENE__H
#define __CARD__SCENE__H

#include "cocos2d.h"
#include <vector>

using namespace cocos2d;
using namespace std;

class CardScene : public Scene {
public:
	static Scene* createScene();

	virtual bool init();

	// Play Cards Logic Functions
	void initCards();
	void useCard(int index);

	// Touch Functions
	bool onTouchBegan(Touch *touch, Event *event);
	void onTouchEnded(Touch *touch, Event *event);
	void onTouchMoved(Touch *touch, Event *event);
	void initAndCleanClick();

	// Mouse Functions
	void onMouseMoved(Event* e);

	// Listeners
	void addTouchListener();
	void addMouseListener();

	// Board Rules
	void initLines();


	CREATE_FUNC(CardScene);

private:
	Vector<Sprite*> cards;
	vector<Vec2> originPos;
	vector<bool> isClick;
	vector<bool> outFlag;
	vector<bool> onBoard;
	vector<int> lineCardNum;

};

#endif