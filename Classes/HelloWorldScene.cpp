#include "HelloWorldScene.h"
#include "NetWorkAccess.h"
#include "SimpleAudioEngine.h"
#include "CardScene.h"
#include "MagicBook.h"

USING_NS_CC;
using namespace CocosDenshion;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    
    if ( !Scene::init() )
    {
        return false;
    }
	Director::getInstance()->getOpenGLView()->setFrameSize(1060, 704);

	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();
    
	username = UserDefault::getInstance()->getStringForKey("username");
	userKey = UserDefault::getInstance()->getStringForKey("userkey");
	std::string head = username + "_head";
	userRick = UserDefault::getInstance()->getIntegerForKey(head.c_str()) - 1;
	std::string headPath = "characters/" + Value(userRick).asString() + "/Rick4.png";
	path = "characters/" + Value(userRick).asString();

	player = Sprite::create(headPath);
	player->setPosition(visibleSize.width / 3, visibleSize.height / 4 + 100);
	player->setScale(0.3);
	this->addChild(player, 1);
	placeTileMap();

	book = Sprite::create("book.png");
	book->setPosition(Vec2(visibleSize.width  - 120, visibleSize.height + 20));
	book->setScale(0.35);
	this->addChild(book, 1);

	initUserInfo();
	initAllUsers();

	addTouchListener();
	addMouseListener();
	addKeyBoardListener();

	preLoadMusic();
	playBgm();

	schedule(schedule_selector(HelloWorld::update), 0.05f, kRepeatForever, 0);
	schedule(schedule_selector(HelloWorld::networkUpdate), 0.02f, kRepeatForever, 0);

	nowMsg = "";
	state = 0;
	outFlag = false;

	auto requestLabel = Label::createWithTTF(username, "fonts/Marker Felt.ttf", 50);
	requestLabel->setPosition(Vec2(70, 160));
	requestLabel->setColor(Color3B::ORANGE);
	player->addChild(requestLabel, 1);

    return true;
}

void HelloWorld::update(float f) {
	showRequestIcon();
	if (isMove) {
		move(movekey);
	}

}

void HelloWorld::initAllUsers() {
	access0.GetAllUserLocation();
}

bool HelloWorld::checkUserIsIn(string username) {
	for (auto e : otherPlayers) {
		if (e.first == username)
			return true;
	}
	return false;
}

void HelloWorld::networkUpdate(float f) {
	string newMsg = access0.getMessage();
	auto root = Director::getInstance()->getRunningScene();
	if (nowMsg != newMsg) {
		nowMsg = newMsg;
		auto res = access0.split(nowMsg, "||");
		if (res.size() == 5 && res[0] != "locs") {
			auto sign = res[0], usrname = res[1];
			int rickType = Value(res[2]).asInt();
			float x = Value(res[3]).asFloat(), y = Value(res[4]).asFloat();
			if (sign == "newUser" && usrname != username) {
				// New User Join in
				addNewUser(usrname, rickType, Vec2(x, y));
			}
			else if (sign == "newLoc" && usrname != username) {
				// Update Old User Locations
				auto targetUser = (Sprite*)root->getChildByName(usrname);
				if (targetUser == NULL) {
					addNewUser(usrname, rickType, Vec2(x, y));
				}
				else {
					targetUser = (Sprite*)root->getChildByName(usrname);
					auto loc = targetUser->getPosition();
					auto rickType = UserDefault::getInstance()->getIntegerForKey(usrname.c_str());
					string path, statePath = usrname + "state";
					auto dirc = 0;
					auto storeState = UserDefault::getInstance()->getIntegerForKey(statePath.c_str());
					if (loc.y != y) {
						if (loc.y < y) {
							dirc = 8;
						}
					}
					else if (loc.x != x) {
						if (loc.x < x) {
							dirc = 12;
						}
						else if(loc.x > x){
							dirc = 4;
						}
					}
					Vector<SpriteFrame*> Move;
					Move.reserve(1);
					char movePath[40];
					path = "characters/" + Value(rickType).asString() + "/Rick%d.png";
					sprintf(movePath, path.c_str(), dirc + storeState);
					auto frame = SpriteFrame::create(movePath, Rect(0, 0, 125, 162));
					Move.pushBack(frame);
					if (storeState == 3) {
						storeState = 0;
					}
					else {
						storeState++;
					}
					UserDefault::getInstance()->setIntegerForKey(statePath.c_str(), storeState);
					auto move = MoveTo::create(0.1f, Vec2(x, y));
					auto animation = Animation::createWithSpriteFrames(Move, 0.2f);
					auto animate = Animate::create(animation);
					auto repeat = Repeat::create(animate, 1);
					targetUser->runAction(repeat);
					targetUser->runAction(move);
				}
			}
		}
		else if (res.size() == 2) {
			if(res[0] == "userLogOut")
				deleteUser(res[1]);
			else {
				MyDialog* dt = MyDialog::create();
				dt->chooseMode(3);
				if (res[0] == "reqDec") {
					// User Not Online
					dt->setTitle("Request Failed");
					this->addChild(dt, 1);
					dt->changeMsg("User " + targetUserName + " Not Online!", false);
				}
				else if (res[0] == "reqPend") {
					// Request successfully send
					dt->setTitle("Request Success");
					this->addChild(dt, 1);
					dt->changeMsg("Request has Successfully \n sent to " + targetUserName + " !", true);
				}
			}
		}
		else if (res.size() == 3) {
			MyDialog* dp = MyDialog::create();
			if (res[0] == "reqAc") {
				// Start Game
				UserDefault::getInstance()->setStringForKey("sessionKey", res[2]);
				targetUserName = res[1];
				dp->setTitle("Are you ready?");
				dp->chooseMode(4);
				dp->setEnterCallBackFunc(this, callfuncN_selector(HelloWorld::jumpToPlayCards));
				this->addChild(dp, 1);
				dp->changeMsg(res[1] + " and you accept the competition,\n Ready to start Schwifty Cards ?", true);
			}
			else if (res[0] == "recReq") {
				// Receive request from others
				if (requestUserName == "")
					requestUserName = res[1];
				else if (requestUserName != res[1])
					return;
				dp->setTitle("Game Request");
				dp->chooseMode(4);
				dp->setEnterCallBackFunc(this, callfuncN_selector(HelloWorld::acceptInvitation));
				this->addChild(dp, 1);
				dp->changeMsg("Accept Invitation From " + res[1] + " ?\n" + res[1] + " has deck count: " + res[2], true);
			}
		}
		else {
			auto sign = res[0];
			int size = res.size();
			if (sign == "locs") {
				// Init All Online Users Locations
				for (int i = 1; i < res.size(); i += 4) {
					string usrname = res[i];
					if (usrname != username) {
						int rickType = Value(res[i + 1]).asInt();
						Vec2 pos = Vec2(Value(res[i + 2]).asFloat(), Value(res[i + 3]).asFloat());
						addNewUser(usrname, rickType, pos);
					}
				}
			}
		}
	}

}

void HelloWorld::addNewUser(string username, int rickType, Vec2 initLoc) {
	if (checkUserIsIn(username))
		return;
	string path = "characters/" + Value(rickType).asString() + "/Rick4.png";
	auto newUser = Sprite::create(path);
	newUser->setScale(0.3);
	string storeState = username + "state";
	UserDefault::getInstance()->setIntegerForKey(username.c_str(), rickType);
	UserDefault::getInstance()->setIntegerForKey(storeState.c_str(), 0);
	newUser->setPosition(initLoc);
	newUser->setName(username);
	auto requestLabel = Label::createWithTTF(username, "fonts/Marker Felt.ttf", 50);
	requestLabel->setPosition(Vec2(70, 160));
	requestLabel->setColor(Color3B::ORANGE);
	newUser->addChild(requestLabel, 1);
	this->addChild(newUser);
	otherPlayers.push_back(pair<string, Sprite*>(username, newUser));
}

void HelloWorld::deleteUser(string username) {
	auto root = Director::getInstance()->getRunningScene();
	auto sceneTarget = (Sprite*)root->getChildByName(username);
	sceneTarget->removeFromParentAndCleanup(true);
	vector<pair<string, Sprite*> >::iterator it = otherPlayers.begin();
	for (; it != otherPlayers.end(); it++) {
		if ((*it).first == username) {
			otherPlayers.erase(it);
			break;
		}
	}
}

void HelloWorld::preLoadMusic() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->preloadBackgroundMusic("music/jazz.mp3");
}

void HelloWorld::playBgm() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->playBackgroundMusic("music/jazz.mp3", true);

}

void HelloWorld::initUserInfo() {
	username = UserDefault::getInstance()->getStringForKey("username");
	userCardsNum = 13;
	std::string headPath = path + "/head.png";

	auto userHead = Sprite::create(headPath);
	userHead->setPosition(Vec2(visibleSize.width - 20, visibleSize.height - 70));
	userHead->setScale(0.3);
	auto name = Label::createWithTTF(username, "fonts/Marker Felt.ttf", 18);
	auto cSize = userHead->getPosition();
	name->setPosition(Vec2(cSize.x, cSize.y - 50));
	
	
	this->addChild(name, 2);
	this->addChild(userHead, 1);
}

void HelloWorld::sendGameRequest() {
	Rect playerRect = player->getBoundingBox();
	Rect requestRect;
	switch (movekey) {
	case 'W':
		requestRect = Rect(playerRect.getMinX(), playerRect.getMinY(),
			playerRect.getMaxX() - playerRect.getMinX(), playerRect.getMaxY() + 30 - playerRect.getMinY());
		break;
	case 'A':
		requestRect = Rect(playerRect.getMinX() - 30, playerRect.getMinY(),
			playerRect.getMaxX() + 30 - playerRect.getMinX(), playerRect.getMaxY() - playerRect.getMinY());
		break;
	case 'S':
		requestRect = Rect(playerRect.getMinX(), playerRect.getMinY() - 30,
			playerRect.getMaxX() - playerRect.getMinX(), playerRect.getMaxY() + 30 - playerRect.getMinY());
		break;
	case 'D':
		requestRect = Rect(playerRect.getMinX(), playerRect.getMinY(),
			playerRect.getMaxX() + 30 - playerRect.getMinX(), playerRect.getMaxY() - playerRect.getMinY());
		break;
	};
	for (auto e : otherPlayers) {
		auto name = e.first;
		auto other = e.second;
		if (requestRect.containsPoint(other->getPosition())) {
			MyDialog* dd = MyDialog::create();
			for (auto e : gameRequests) {
				if (e.first == name && e.second == true) {
					dd->setTitle("Warning");
					dd->chooseMode(3);
					this->addChild(dd, 1);
					dd->changeMsg("You have already send request to \n" + name + " ! Please wait for response !", false);
					return;
				}
			}
			targetUserName = name;
			dd->chooseMode(2);
			dd->setTitle("Invitation Confirm");
			dd->setEnterCallBackFunc(this, callfuncN_selector(HelloWorld::sendNetWorkRequest));
			dd->setName("Invitation Confirm");
			this->addChild(dd, 1);
			dd->changeMsg("Do you want to invite " + name + "\nto start playing Schwifty Cards?", true);
			return;
		}
	}
}

void HelloWorld::showRequestIcon() {
	Rect playerRect = player->getBoundingBox();
	Rect requestRect;
	switch (movekey) {
	case 'W':
		requestRect = Rect(playerRect.getMinX(), playerRect.getMinY(),
			playerRect.getMaxX() - playerRect.getMinX(), playerRect.getMaxY() + 30 - playerRect.getMinY());
		break;
	case 'A':
		requestRect = Rect(playerRect.getMinX() - 30, playerRect.getMinY(),
			playerRect.getMaxX() + 30 - playerRect.getMinX(), playerRect.getMaxY() - playerRect.getMinY());
		break;
	case 'S':
		requestRect = Rect(playerRect.getMinX(), playerRect.getMinY() - 30,
			playerRect.getMaxX() - playerRect.getMinX(), playerRect.getMaxY() + 30 - playerRect.getMinY());
		break;
	case 'D':
		requestRect = Rect(playerRect.getMinX(), playerRect.getMinY(),
			playerRect.getMaxX() + 30 - playerRect.getMinX(), playerRect.getMaxY() - playerRect.getMinY());
		break;
	};
	for (auto e : otherPlayers) {
		string name = e.first;
		Sprite* other = e.second;
		if (requestRect.containsPoint(other->getPosition())) {
			if (other->getChildByName("space") != NULL)
				return;
			auto space = Sprite::create("space0.png");
			space->setScale(0.65);
			Vector<SpriteFrame*> blink;
			blink.reserve(2);
			for (int i = 0; i < 2; i++) {
				string path = "space" + Value(1 - i).asString() + ".png";
				auto frame = SpriteFrame::create(path, Rect(0, 0, 434, 317));
				blink.pushBack(frame);
			}
			auto animation = Animation::createWithSpriteFrames(blink, 0.5f);
			auto animate = Animate::create(animation);
			space->setPosition(Vec2(60, 180));
			space->runAction(RepeatForever::create(animate));
			space->setName("space");
			other->removeAllChildrenWithCleanup(true);
			other->addChild(space, 1);
		}
		else {
			other->removeAllChildrenWithCleanup(true);
			auto requestLabel = Label::createWithTTF(name, "fonts/Marker Felt.ttf", 50);
			requestLabel->setPosition(Vec2(70, 160));
			requestLabel->setColor(Color3B::ORANGE);
			other->addChild(requestLabel, 1);
		}
	}
}

void HelloWorld::sendNetWorkRequest(Node* node) {
	access0.InviteToGame(userKey, targetUserName);
	gameRequests.push_back(pair<string, bool>(targetUserName, true));
	auto root = Director::getInstance()->getRunningScene();
	if (root->getChildByName("Invitation Confirm") != NULL)
		root->getChildByName("Invitation Confirm")->removeFromParentAndCleanup(true);
}

void HelloWorld::jumpToPlayCards(Node* node) {
	UserDefault::getInstance()->setStringForKey("username", username);
	UserDefault::getInstance()->setStringForKey("oppoUsername", targetUserName);
	this->setVisible(false);
	Scene* s = CardScene::createScene();
	auto animate = TransitionFade::create(0.4f, s);
	Director::getInstance()->replaceScene(animate);
}

void HelloWorld::acceptInvitation(Node* node) {
	access0.AcceptInvitation(userKey, requestUserName);
}

void HelloWorld::move(char dir) {
	if (player->getNumberOfRunningActions() > 0) {
		return;
	}
	auto dirc = 0;
	auto offset = Vec2(0, 0);
	switch (dir) {
		case 'W':
			offset.y = 10;
			dirc = 8;
			break;
		case 'A':
			offset.x = -10;
			dirc = 4;
			break;
		case 'S':
			offset.y = -10;
			break;
		case 'D':
			offset.x = 10;
			dirc = 12;
	}

	Vector<SpriteFrame*> Move;
	Move.reserve(1);
	char movePath[40];
	std::string realPath = path + "/Rick%d.png";
	sprintf(movePath, realPath.c_str(), dirc + state);
	auto frame = SpriteFrame::create(movePath, Rect(0, 0, 125, 162));
	Move.pushBack(frame);
	if (state == 3) {
		state = 0;
	}
	else {
		state++;
	}

	auto dest = offset + player->getPosition();
	if (dest.x < 55)
		dest.x = 60;
	else if (dest.x > visibleSize.width + 30)
		dest.x = visibleSize.width + 30;
	if (dest.y < 20)
		dest.y = 20;
	else if (dest.y > visibleSize.height - 20)
		dest.y = visibleSize.height - 20;
		
	auto move = MoveTo::create(0.1f, dest);
	auto animation = Animation::createWithSpriteFrames(Move, 0.2f);
	auto animate = Animate::create(animation);

	auto repeat = Repeat::create(animate, 1);
	player->runAction(repeat);
	player->runAction(move);
	access0.UpdateUserLocation(userKey, userRick, player->getPosition().x, player->getPosition().y);
}

void HelloWorld::placeTileMap() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	TMXTiledMap* map = TMXTiledMap::create("test.tmx");
	map->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	map->setAnchorPoint(Vec2(0.5, 0.5));
	this->addChild(map, 0);
}

void HelloWorld::addTouchListener() {
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegin, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void HelloWorld::addMouseListener() {
	auto mouseListener = EventListenerMouse::create();
	mouseListener->onMouseMove = CC_CALLBACK_1(HelloWorld::onMouseMoved, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouseListener, book);
}

void HelloWorld::addKeyBoardListener() {
	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPress, this);
	keyListener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyReleased, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyListener, this);
}

void HelloWorld::onMouseMoved(Event* e) {
	EventMouse* ee = (EventMouse*)e;
	Vec2 pos = Vec2(ee->getCursorX(), ee->getCursorY());
	if (book->getPosition().getDistance(pos) <= 40) {
		if (!outFlag) {
			outFlag = true;
			auto animate = MoveTo::create(1.0f, Vec2(book->getPosition().x, book->getPosition().y - 50));
			book->runAction(animate);
		}
	}
	else {
		if (outFlag) {
			auto animate = MoveTo::create(1.0f, Vec2(book->getPosition().x, book->getPosition().y + 50));
			book->runAction(animate);
		}
		outFlag = false;
	}
}

void HelloWorld::onTouchMoved(Touch* touchs, Event* event) {}

bool HelloWorld::onTouchBegin(Touch* touch, Event* event) {
	if (book->getBoundingBox().containsPoint(touch->getLocation())) {
		this->setVisible(false);
		auto s = MagicBook::createScene();
		auto fade = TransitionFade::create(0.4f, s);
		Director::getInstance()->replaceScene(fade);
	}
	return true;
}

void HelloWorld::onKeyPress(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_W:
	case EventKeyboard::KeyCode::KEY_W:
		movekey = 'W';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_A:
		movekey = 'A';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_S:
	case EventKeyboard::KeyCode::KEY_S:
		movekey = 'S';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
	case EventKeyboard::KeyCode::KEY_D:
		movekey = 'D';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_SPACE:
		sendGameRequest();
		break;
	}
}

void HelloWorld::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	player->stopAllActions();
	switch (code) {
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_W:
	case EventKeyboard::KeyCode::KEY_W:
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_A:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_S:
	case EventKeyboard::KeyCode::KEY_S:
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_D:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
		isMove = false;
		break;
	}
}

