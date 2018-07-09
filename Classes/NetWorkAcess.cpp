#include "NetWorkAccess.h"


NetWorkAccess::NetWorkAccess() {
	getMsg = "";
	webSocketURL = "ws://esblog.chenmt.science/schwifty";
	m_wSocket = new WebSocket();
	m_wSocket->init(*this, webSocketURL);
}

// WebSocket Connection
void NetWorkAccess::onOpen(WebSocket* ws) {
	CCLOG("WebSocket connect success !");
}

void NetWorkAccess::onMessage(WebSocket* ws, const WebSocket::Data& data) {
	char str[500];
	sprintf(str, "%s", data.bytes);
	CCLOG(str);
	getMsg = str;
}

void NetWorkAccess::onClose(WebSocket* ws) {
	CCLOG("WebSocket connect close !");
	if (ws == m_wSocket) {
		m_wSocket = NULL;
	}
}

void NetWorkAccess::onError(WebSocket* ws, const WebSocket::ErrorCode& errCode) {
	CCLOG("WebSocket connect error !");
}

// Basic User Log Functions
void NetWorkAccess::AttemptLogin(string username, string password) {
	string sendStr = "login&&" + username + "&&" + password;
	m_wSocket->send(sendStr);
}

// Basic Game Invitation Functions
void NetWorkAccess::InviteToGame(string userKey, string targetUserName) {
	string sendStr = "gameReq&&" + userKey + "&&" + targetUserName;
	m_wSocket->send(sendStr);
}

void NetWorkAccess::AcceptInvitation(string userKey, string senderUserName) {
	string sendStr = "accReq&&" + userKey + "&&" + senderUserName;
	m_wSocket->send(sendStr);
}

void NetWorkAccess::GetAllInvitations(string userKey) {
	string sendStr = "allReq&&" + userKey;
	m_wSocket->send(sendStr);
}


// Decks Functions
void NetWorkAccess::GetAllDecks(string username) {
	string sendStr = "getAllDeck&&" + username;
	m_wSocket->send(sendStr);
	
}

void NetWorkAccess::PlayOutaCard(string sessionKey, string cardIndex) {
	string sendStr = "play&&" + sessionKey + "&&" + cardIndex;
	m_wSocket->send(sendStr);
	
}

void NetWorkAccess::EndRound(string sessionKey) {
	string sendStr = "endRound&&" + sessionKey;
	m_wSocket->send(sendStr);
}

void NetWorkAccess::GetAttackPoints(string sessionKey, string username) {
	string sendStr = "attackPt&&" + sessionKey + "&&" + username;
	m_wSocket->send(sendStr);
}

void NetWorkAccess::GetCurrentStatus(string sessionKey) {
	string sendStr = "getStatus&&" + sessionKey;
	m_wSocket->send(sendStr);
}

void NetWorkAccess::GetRound(string sessionKey, string username) {
	string sendStr = "getRound&&" + sessionKey + "&&" + username;
	m_wSocket->send(sendStr);
}

// Moving Update Functions
void NetWorkAccess::UpdateUserLocation(string userKey, int rickType, float x, float y) {
	string sendStr = "update&&" + userKey + "&&" + Value(rickType).asString() +
						"&&" + Value(x).asString() + "&&" + Value(y).asString();
	m_wSocket->send(sendStr);
}

void NetWorkAccess::GetAllUserLocation() {
	m_wSocket->send("allLoc");
}

string NetWorkAccess::getMessage() {
	return getMsg;
}

vector<string> NetWorkAccess::split(const string& s, const string& delim) {
	vector<string> elems;
	size_t pos = 0;
	size_t len = s.length();
	size_t delim_len = delim.length();
	if (delim_len == 0) return elems;
	while (pos < len)
	{
		int find_pos = s.find(delim, pos);
		if (find_pos < 0)
		{
			elems.push_back(s.substr(pos, len - pos));
			break;
		}
		elems.push_back(s.substr(pos, find_pos - pos));
		pos = find_pos + delim_len;
	}
	return elems;
}

// Parse Functions
pair<bool, string> NetWorkAccess::ParseLogin(string logRes) {
	vector<string> res = split(logRes, "||");
	if (res[0] == "login") {
		return pair<bool, string>(true, res[1]);
	}
	else {
		return pair<bool, string>(false, res[1]);
	}
}

NetWorkAccess access0 = NetWorkAccess();