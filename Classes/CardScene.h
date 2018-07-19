#pragma once
#ifndef __CARD__SCENE__H
#define __CARD__SCENE__H

#include "cocos2d.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "NetWorkAccess.h"
#include "MyDialog.h"
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
	void initMyCards(vector<string> res);
	void playOutACard(int i);
	void addOppoCard(int index);
	void endMyRound(Ref* e);
	void showMyTurnSign();
	void allEndThisRound();
	void startNewRound();
	void endCardScene();

	// NetWork Listener
	void networkUpdate(float f);

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
	void initGameDatas();
	void initPointLabels();
	void changeBoardState(bool flag);
	void changePoints(int row, int point);

	// Read Cards Roles
	void initJSONDetails();
	Card* getCardByName(string cardname);
	Card* getCardByID(int id);


	CREATE_FUNC(CardScene);

private:
	vector<pair<int, Sprite*> > cards;
	vector<pair<int, string> > cardNames;
	vector<Sprite*> opponentCards;
	vector<Vec2> originPos;
	vector<bool> isClick;
	vector<bool> outFlag;
	vector<bool> OppOutFlag;
	vector<bool> onBoard;
	vector<int> lineCardNum;
	vector<int> MyLinePoints; // 0 1 2 | 3 4 5
	vector<Card*> jsonDetails;
	vector<Sprite*> allLabels;
	vector<Sprite*> myBoardCards;
	vector<Sprite*> oppoBoardCards;

	// Coin characters And Scale animations
	Sprite* bCoin;
	Sprite* rCoin;
	Sprite* default0;
	Sprite* endRoundSign;
	Sprite* myTurnSign;
	Sprite* myTrash;
	Sprite* oppoTrash;
	Label* targetCardName;
	Label* targetCardType;
	bool coinState;

	// Basic Infos of Users
	ParticleSystem* myBlueBoardLeft;
	ParticleSystem* myBlueBoardRight;
	ParticleSystem* oppoRedBoradLeft;
	ParticleSystem* oppoRedBoradRight;
	string myName;
	string oppoName;
	string sessionKey;
	int myRickType;
	int oppoRickType;
	int myPoints;
	int oppoPoints;
	int handCardsNum;

	// NetWork Message
	string nowMsg;
	bool nowRoundState;
	bool oppoRoundState;
	int roundNum;

};

#endif