#include "CardScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* CardScene::createScene()
{
	return CardScene::create();
}


bool CardScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	Director::getInstance()->getOpenGLView()->setFrameSize(1400, 850);
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();
	
	auto bg = Sprite::create("board.png");
	bg->setPosition(Vec2(visibleSize.width / 2, visibleSize.height /2));
	bg->setScale(visibleSize.width / bg->getContentSize().width,
		visibleSize.height / bg->getContentSize().height);
	this->addChild(bg, 0);

	for (int i = 0; i < 13; i++) {
		outFlag.push_back(false);
		onBoard.push_back(false);
	}
	for (int i = 0; i < 3; i++) {
		lineCardNum.push_back(0);
	}

	addTouchListener();
	addMouseListener();

	bCoin = Sprite::create("coin0.png");
	rCoin = Sprite::create("coin1.png");
	bCoin->setPosition(Vec2(visibleSize.width / 15 - 15, visibleSize.height / 2 + 30));
	bCoin->setScale(0.18);
	rCoin->setPosition(bCoin->getPosition());
	rCoin->setScale(0.18);
	rCoin->setVisible(false);
	this->addChild(rCoin, 1);
	this->addChild(bCoin, 1);

	schedule(schedule_selector(CardScene::networkUpdate), 0.02f, kRepeatForever, 0);

	nowMsg = "";
	sessionKey = UserDefault::getInstance()->getStringForKey("sessionKey");
	access0.GetCurrentDecks(sessionKey);
	coinState = true;

	initGameDatas();
	initJSONDetails();
	initAndCleanClick();
	initLines();

	default0 = Sprite::create("default.png");
	default0->setPosition(Vec2(visibleSize.width - 120, visibleSize.height - 120));
	default0->setScale(0.5);
	this->addChild(default0, 1);

	auto cardPos = default0->getPosition();
	targetCardName = Label::createWithTTF(" ", "fonts/Marker Felt.ttf", 20);
	targetCardType = Label::createWithTTF(" ", "fonts/Marker Felt.ttf", 16);
	targetCardName->setPosition(Vec2(cardPos.x, cardPos.y - 260));
	targetCardType->setPosition(Vec2(cardPos.x, cardPos.y - 200));
	targetCardType->setColor(Color3B::ORANGE);
	this->addChild(targetCardName, 1);
	this->addChild(targetCardType, 1);
	
	return true;
}

void CardScene::initMyCards(vector<string> res) {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	// Init User's Cards
	for (int i = 2; i < res.size(); i++) {
		int id = Value(res[i]).asInt();
		auto result = getCardByID(id);
		string path0 = "characters/Decks/" + result->type + "/" + result->name + ".png";
		auto temp = Sprite::create(path0);
		cards.push_back(pair<int, Sprite*>(i - 2, temp));
		cardNames.push_back(pair<int, string>(i - 2, result->name));
	}

	// Show cards
	int borderWidth = 70;
	int i = 0;
	for (auto item : cards) {
		auto it = item.second;
		it->setScale(0.17);
		it->setPosition(Vec2(borderWidth, 20));
		it->setTag(i);
		this->addChild(it, 1);
		borderWidth += 68;
		originPos.push_back(it->getPosition());
		i++;
	}

}

void CardScene::networkUpdate(float f) {
	string newMsg = access0.getMessage();
	auto root = Director::getInstance()->getRunningScene();
	if (nowMsg != newMsg) {
		nowMsg = newMsg;
		auto res = access0.split(nowMsg, "||");
		if (res[0] == "deck") {
			if (res[1] == myName) {
				initMyCards(res);
			}
		}
	}
}

void CardScene::initLines() {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	int order_height = 110;
	for (int i = 0; i < 3; i++) {
		auto sp1 = Sprite::create("line.png");
		sp1->setPosition(Vec2(visibleSize.width / 2 - 40, order_height));
		sp1->setScale(0.5);
		sp1->setName("line" + Value(i).asString());
		sp1->setContentSize(Size(1000, sp1->getContentSize().height));
		order_height += sp1->getContentSize().height - 65;
		sp1->setVisible(false);
		this->addChild(sp1, 1);
	}
}

void CardScene::initGameDatas() {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	myName = UserDefault::getInstance()->getStringForKey("username");
	oppoName = UserDefault::getInstance()->getStringForKey("oppoUsername");
	string path = myName + "_head";
	myRickType = UserDefault::getInstance()->getIntegerForKey(path.c_str()) - 1;
	oppoRickType = UserDefault::getInstance()->getIntegerForKey(oppoName.c_str());

	// Adding Characters Cards
	auto myRickCard = Sprite::create("characters/" + Value(myRickType).asString() + "/head.png");
	auto oppoRickCard = Sprite::create("characters/" + Value(oppoRickType).asString() + "/head.png");
	auto myNameLabel = Label::createWithTTF(myName, "fonts/Marker Felt.ttf", 40);
	auto oppoNameLabel = Label::createWithTTF(oppoName, "fonts/Marker Felt.ttf", 40);
	myRickCard->setScale(0.28);
	oppoRickCard->setScale(0.28);
	myRickCard->addChild(myNameLabel, 1);
	oppoRickCard->addChild(oppoNameLabel, 1);
	myRickCard->setPosition(Vec2(55, visibleSize.height / 5 + 50));
	oppoRickCard->setPosition(Vec2(55, visibleSize.height - visibleSize.height / 5));
	myNameLabel->setPosition(Vec2(myRickCard->getContentSize().width / 2, 80));
	oppoNameLabel->setPosition(Vec2(oppoRickCard->getContentSize().width / 2, 80));
	this->addChild(myRickCard, 1);
	this->addChild(oppoRickCard, 1);

	// Adding Blue blinks
	myBlueBoardLeft = ParticleMeteor::create();
	myBlueBoardRight = ParticleMeteor::create();
	myBlueBoardLeft->setTexture(Director::getInstance()->getTextureCache()->addImage("blue.jpg"));
	myBlueBoardRight->setTexture(Director::getInstance()->getTextureCache()->addImage("blue.jpg"));
	myBlueBoardLeft->setPosition(Vec2(visibleSize.width / 2, -10));
	myBlueBoardRight->setPosition(myBlueBoardLeft->getPosition());
	myBlueBoardLeft->setRotation(-45);
	this->addChild(myBlueBoardLeft, 0);
	myBlueBoardRight->setRotation(135);
	this->addChild(myBlueBoardRight, 0);

	// Adding Red blinks
	oppoRedBoradLeft = ParticleFire::create();
	oppoRedBoradRight = ParticleFire::create();
	oppoRedBoradLeft->setTexture(Director::getInstance()->getTextureCache()->addImage("fire.png"));
	oppoRedBoradRight->setTexture(Director::getInstance()->getTextureCache()->addImage("fire.png"));
	oppoRedBoradLeft->setPosition(Vec2(visibleSize.width / 2 - 70, visibleSize.height + 35));
	oppoRedBoradLeft->setRotation(-90);
	oppoRedBoradRight->setPosition(Vec2(visibleSize.width / 2, visibleSize.height + 35));
	oppoRedBoradRight->setRotation(90);
	oppoRedBoradLeft->setVisible(false);
	oppoRedBoradRight->setVisible(false);
	this->addChild(oppoRedBoradLeft, 0);
	this->addChild(oppoRedBoradRight, 0);
}

void CardScene::initJSONDetails() {
	string data = FileUtils::getInstance()->getStringFromFile("json/cardset.json");
	rapidjson::Document doc;
	doc.Parse(data.c_str());
	auto cardArr = doc.GetArray();
	for (auto& obj : cardArr) {
		auto store = new Card(obj["attack"].GetInt(), obj["type"].GetString(), obj["name"].GetString(), obj["index"].GetInt());
		jsonDetails.push_back(store);
	}
}

void CardScene::addTouchListener() {
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(CardScene::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(CardScene::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(CardScene::onTouchEnded, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

bool CardScene::onTouchBegan(Touch *touch, Event *event) {
	initAndCleanClick();
	auto root = Director::sharedDirector()->getRunningScene();
	for (int i = 0; i < 13; i++) {
		Sprite* temp = (Sprite*)root->getChildByTag(i);
		if (touch->getLocation().getDistance(temp->getPosition()) <= 40) {
			isClick[i] = true;
		}
	}
	return true;
}

void CardScene::onTouchEnded(Touch *touch, Event *event) {
	int target = -1;
	bool correctFlag = false;
	auto visibleSize = Director::getInstance()->getVisibleSize();
	for (int i = 0; i < 13; i++) {
		if (isClick[i]) {
			target = i;
			break;
		}
	}
	if (target == -1) {
		return;
	}
	auto root = Director::getInstance()->getRunningScene();
	auto cardPos = (Sprite*)root->getChildByTag(target);
	if (onBoard[target] == true) {
		cardPos->setPosition(originPos[target]);
		return;
	}
	for (int i = 0; i < 3; i++) {
		auto temp = (Sprite*)root->getChildByName("line" + Value(i).asString());
		auto judge_height = 80;
		if (i == 2)
			judge_height = 60;
		else if (i == 0)
			judge_height = 110;
		if (temp->getBoundingBox().containsPoint(cardPos->getPosition())) {
			cardPos->setPosition(Vec2(visibleSize.width / 2 - 250 + lineCardNum[i]*60, judge_height + 100*i));
			onBoard[target] = true;
			originPos[target] = cardPos->getPosition();
			correctFlag = true;
			lineCardNum[i]++;
			auto upMove = Sequence::create(DelayTime::create(0.5), Show::create(),
				OrbitCamera::create(0.5, 1.5, 0, 180, 90, 0, 0), NULL);
			auto downMove = Sequence::create(OrbitCamera::create(0.5, 1.5, 0, 0, 90, 0, 0), Hide::create(),
				DelayTime::create(0.5), NULL);

			if (coinState) {
				bCoin->runAction(downMove);
				rCoin->runAction(upMove);
				changeBoardState(false);
				coinState = false;
			}
			else {
				bCoin->runAction(upMove);
				rCoin->runAction(downMove);
				changeBoardState(true);
				coinState = true;
			}
		}
	}
	if (!correctFlag) {
		cardPos->setPosition(originPos[target]);
	}
	initAndCleanClick();
    for (int i = 0; i < outFlag.size(); i++) {
		outFlag[i] = false;
	}

}

void CardScene::changeBoardState(bool flag) {
	if (flag) {
		myBlueBoardLeft->setVisible(true);
		myBlueBoardRight->setVisible(true);
		oppoRedBoradLeft->setVisible(false);
		oppoRedBoradRight->setVisible(false);
	}
	else {
		myBlueBoardLeft->setVisible(false);
		myBlueBoardRight->setVisible(false);
		oppoRedBoradLeft->setVisible(true);
		oppoRedBoradRight->setVisible(true);
	}
}

void CardScene::onTouchMoved(Touch *touch, Event *event) {
	for (int i = 0; i < 13; i++) {
		if (isClick[i]) {
			auto delta = touch->getDelta();
			Sprite* temp = NULL;
			for (auto& e : cards) {
				if (e.first == i)
					temp = e.second;
			}
			temp->setPosition(temp->getPosition() + delta);
		}
	}
}

void CardScene::initAndCleanClick() {
	if (isClick.size() == 0) {
		for (int i = 0; i < 13; i++) {
			isClick.push_back(false);
		}
	}
	else {
		for (int i = 0; i < 13; i++) {
			isClick[i] = false;
		}
	}
}

void CardScene::addMouseListener() {
	auto mouseListener = EventListenerMouse::create();
	mouseListener->onMouseDown = CC_CALLBACK_1(CardScene::onMouseDown, this);
	mouseListener->onMouseMove = CC_CALLBACK_1(CardScene::onMouseMoved, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouseListener, this);
}

void CardScene::onMouseMoved(Event* e) {
	EventMouse* ee = (EventMouse*)e;
	auto root = Director::getInstance()->getRunningScene();
	Vec2 pos = Vec2(ee->getCursorX(), ee->getCursorY());
	for (int i = 0; i < 13; i++) {
		Sprite* sp = (Sprite*)root->getChildByTag(i);
		if (sp->getPosition().getDistance(pos) <= 40) {
			if (!outFlag[i]) {
				outFlag[i] = true;
				auto animate = ScaleTo::create(1.0f, 0.20f, 0.20f);
				sp->runAction(animate);
			}
		}
		else {
			if (outFlag[i]) {
				auto animate = ScaleTo::create(1.0f, 0.17f, 0.17f);
				sp->runAction(animate);
			}
			outFlag[i] = false;
		}
	}
}

Card* CardScene::getCardByName(string cardname) {
	for (auto e : jsonDetails) {
		if (e->name == cardname)
			return e;
	}
}

Card* CardScene::getCardByID(int id) {
	for (auto e : jsonDetails) {
		if (e->index == id)
			return e;
	}
}

void CardScene::onMouseDown(Event* e) {
	EventMouse* ee = (EventMouse*)e;
	auto root = Director::getInstance()->getRunningScene();
	Vec2 pos = Vec2(ee->getCursorX(), ee->getCursorY());
	for (int i = 0; i < 13; i++) {
		Sprite* sp = (Sprite*)root->getChildByTag(i);
		string name = cardNames[i].second;
		Card* target = getCardByName(name);
		if (sp->getPosition().getDistance(pos) <= 50) {
			string path0 = "characters/Decks/" + target->type + "/" + target->name + ".png";
			default0->setTexture(Director::getInstance()->getTextureCache()->addImage(path0));
			targetCardName->setString(target->name);
			targetCardType->setString(target->type);
		}
	}
}
