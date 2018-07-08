#pragma once
#ifndef __NETWORK__ACCESS__H__
#define __NETWORK__ACCESS__H__

#include "cocos2d.h"
#include "network/HttpClient.h"
#include "network//WebSocket.h"
#include <iostream>
#include <cstring>
#include <vector>

using namespace std;
using namespace cocos2d;
using namespace cocos2d::network;

class NetWorkAccess : public WebSocket::Delegate{
public:
	NetWorkAccess();

	/*
	* Basic Sending andReceiving Functions
	*/

	// WebSocket Connection
	void onOpen(WebSocket* ws);
	void onMessage(WebSocket* ws, const WebSocket::Data& data);
	void onClose(WebSocket* ws);
	void onError(WebSocket* ws, const WebSocket::ErrorCode& errCode);

	// Basic User Log Functions
	void AttemptLogin(string username, string password);

	// Basic Game Invitation Functions
	void InviteToGame(string userKey, string targetUserName);
	void AcceptInvitation(string userKey, string senderUserName);
	void GetAllInvitations(string userKey);

	// Decks Functions
	void GetAllDecks(string username);
	void PlayOutaCard(string sessionKey, string cardIndex);
	void EndRound(string sesssionKey);
	void GetAttackPoints(string sessionKey, string username);

	// Users Moving Functions
	void UpdateUserLocation(string userKey, int rickType, float x, float y);
	void GetAllUserLocation();

	// WebSocket Message BroadCast
	string getMessage();

	/*
	* Return Message Parse Functions
	*/

	// Parse Login
	pair<bool ,string> ParseLogin(string logRes);

	// Parse Movement
	void ParseUpdate(string moveRes);

	vector<string> split(const string& s, const string& delim);
private:
	
	string webSocketURL;
	WebSocket* m_wSocket;
	string getMsg;

};

extern NetWorkAccess access0;
#endif
