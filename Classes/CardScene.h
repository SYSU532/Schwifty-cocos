#pragma once
#ifndef __CARD__SCENE__H
#define __CARD__SCENE__H

#include "cocos2d.h"


using namespace cocos2d;
class CardScene : public Scene {
public:
	static Scene* createScene();

	virtual bool init();

	// Play Cards Logic Functions
	void initCards();
	void useCard(int index);


	CREATE_FUNC(CardScene);

private:
	Vector<Sprite*> cards;

};

#endif