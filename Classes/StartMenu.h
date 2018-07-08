#pragma once
#ifndef __START__MENU__H__
#define __START__MENU__H__

#include "cocos2d.h"
#include "NetWorkAccess.h"
#include "network/HttpClient.h"
#include "json/rapidjson.h"
#include "json/document.h"

using namespace cocos2d;
using namespace cocos2d::network;
using namespace std;

class StartMenu : public Scene {
public:
	static Scene* createScene();

	virtual bool init();

	// Basic Function
	void signin(Ref* pSender);
	void signup(Ref* pSender);

	// Listen to Login Response
	void LogUpdate(float f);

	// Http Acess
	void onLoginAdmit(Node* pNode);
	void onLogupAdmit(Node* pNode);
	void onLogupResponse(HttpClient* sender, HttpResponse* res);

	// Pick Rick
	void pickRick();

	// Music
	void playBgm();
	
	CREATE_FUNC(StartMenu);

private:
	string nowMsg;
	string userKey;

};

#endif
