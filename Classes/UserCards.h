#pragma once
#ifndef __USER__CARDS__H__
#define __USER_CARDS__H__

#include "cocos2d.h"

using namespace cocos2d;

class UserCards : public Scene {
public:
	static Scene* createScene();

	virtual bool init();

	void goBack();

private:


};


#endif