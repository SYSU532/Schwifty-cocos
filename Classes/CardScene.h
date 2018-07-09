#pragma once
#ifndef __CARD__SCENE__H
#define __CARD__SCENE__H

#include "cocos2d.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include <vector>

using namespace cocos2d;
using namespace std;

struct Card {
	int attack;
	string type;
	string name;
	int index;
	Card(int att, string ty, string na, int id) {
		attack = att;
		type = ty;
		name = na;
		index = id;
	}
};

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
	void onMouseDown(Event* e);

	// Listeners
	void addTouchListener();
	void addMouseListener();

	// Board Rules
	void initLines();

	// Read Cards Roles
	void initJSONDetails();
	Card* getCardByName(string cardname);


	CREATE_FUNC(CardScene);

private:
	vector<pair<int, Sprite*> > cards;
	vector<pair<int, string> > cardNames;
	vector<Vec2> originPos;
	vector<bool> isClick;
	vector<bool> outFlag;
	vector<bool> onBoard;
	vector<int> lineCardNum;
	vector<Card*> jsonDetails;

	// Coin characters And Scale animations
	Sprite* bCoin;
	Sprite* rCoin;
	Sprite* default0;
	Label* targetCardName;
	Label* targetCardType;
	bool coinState;

};

#endif