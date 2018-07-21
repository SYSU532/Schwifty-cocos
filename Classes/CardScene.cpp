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
	visibleSize = Director::getInstance()->getVisibleSize();
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
	for (int i = 0; i < 6; i++) {
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
	nowRoundState = true;
	sessionKey = UserDefault::getInstance()->getStringForKey("sessionKey");
	access0.GetCurrentDecks(sessionKey);
	coinState = true;
	roundNum = 1;
	nowRoundState = true;
	oppoRoundState = true;

	initGameDatas();
	initJSONDetails();
	initAndCleanClick();
	initLines();
	initPointLabels();
	preloadMusic();

	default0 = Sprite::create("default.png");
	default0->setPosition(Vec2(visibleSize.width - 130, visibleSize.height - 180));
	default0->setScale(0.5);
	this->addChild(default0, 1);

	auto endRoundLabel = Label::createWithTTF("You have ended This Round!", "fonts/Marker Felt.ttf", 35);
	auto myTurnLabel = Label::createWithTTF("Your Turn to play out a Morty Card!", "fonts/Marker Felt.ttf", 35);
	endRoundSign = Sprite::create("blueSign.png");
	myTurnSign = Sprite::create("blueSign.png");
	endRoundSign->setPosition(visibleSize / 2);
	myTurnSign->setPosition(visibleSize / 2);
	endRoundLabel->setPosition(endRoundSign->getContentSize() / 2);
	myTurnLabel->setPosition(myTurnSign->getContentSize() / 2);
	endRoundSign->setScale(0.5);
	myTurnSign->setScale(0.5);
	endRoundSign->addChild(endRoundLabel, 1);
	myTurnSign->addChild(myTurnLabel, 1);
	endRoundSign->setVisible(false);
	myTurnSign->setVisible(false);
	this->addChild(endRoundSign, 2);
	this->addChild(myTurnSign, 2);

	auto endBtn = MenuItemImage::create("endBtn0.png", "endBtn.png", CC_CALLBACK_1(CardScene::endMyRound, this));
	endBtn->setPosition(Vec2(visibleSize.width - 120, 110));
	endBtn->setScale(0.34);
	auto endLabel = Label::createWithTTF("End Your Round", "fonts/Marker Felt.ttf", 40);
	endLabel->setPosition(Vec2(endBtn->getContentSize().width / 2, endBtn->getContentSize().height / 2));
	endBtn->addChild(endLabel, 1);
	auto menu = Menu::create(endBtn, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	auto cardPos = default0->getPosition();
	targetCardName = Label::createWithTTF(" ", "fonts/Marker Felt.ttf", 20);
	targetCardType = Label::createWithTTF(" ", "fonts/Marker Felt.ttf", 16);
	targetCardName->setPosition(Vec2(cardPos.x, cardPos.y - 200));
	targetCardType->setPosition(Vec2(cardPos.x, cardPos.y - 150));
	targetCardType->setColor(Color3B::ORANGE);
	this->addChild(targetCardName, 1);
	this->addChild(targetCardType, 1);

	// Also init opponent's Cards
	int borderWidth = 70;
	for (int i = 0; i < 13; i++) {
		auto temp = Sprite::create("back.png");
		temp->setScale(0.21);
		temp->setPosition(Vec2(borderWidth, visibleSize.height - 5));
		this->addChild(temp, 1);
		borderWidth += 60;
		opponentCards.push_back(temp);
		OppOutFlag.push_back(false);
	}

	// Adding Trash Cards
	myTrash = Sprite::create("trash.png");
	oppoTrash = Sprite::create("trash.png");
	myTrash->setPosition(Vec2(visibleSize.width - 30, 10));
	oppoTrash->setPosition(Vec2(visibleSize.width - 30, visibleSize.height - 10));
	myTrash->setScale(0.8);
	oppoTrash->setScale(0.8);
	this->addChild(myTrash, 1);
	this->addChild(oppoTrash, 1);

	myOriginFirst = true;

	return true;
}

void CardScene::preloadMusic() {
	myAudio = SimpleAudioEngine::getInstance();
	myAudio->preloadEffect("music/kick.mp3");
	myAudio->preloadBackgroundMusic("music/victory.mp3");
	myAudio->preloadBackgroundMusic("music/defeat.mp3");
	myAudio->preloadBackgroundMusic("music/play.mp3");
	// Change Background music
	myAudio->playBackgroundMusic("music/play.mp3", true);
	myAudio->setBackgroundMusicVolume(0.4);
}

void CardScene::endMyRound(Ref* r) {
	if (!coinState)
		return;
	auto upMove = Sequence::create(DelayTime::create(0.5), Show::create(),
		OrbitCamera::create(0.5, 1.5, 0, 180, 90, 0, 0), NULL);
	auto downMove = Sequence::create(OrbitCamera::create(0.5, 1.5, 0, 0, 90, 0, 0), Hide::create(),
		DelayTime::create(0.5), NULL);
	access0.EndRound(sessionKey);
	nowRoundState = false; // User End this Round
	if (oppoRoundState == false)
		myOriginFirst = true;
	else
		myOriginFirst = false;
	coinState = false;
	changeBoardState(false);
	bCoin->runAction(downMove);
	rCoin->runAction(upMove);
	// Test Animation
	auto endAnimate = Sequence::create(Show::create(),
		ScaleTo::create(0.2, 0.6), DelayTime::create(0.8), Hide::create(), ScaleTo::create(0.1, 0.5), NULL);
	endRoundSign->runAction(endAnimate);

}

void CardScene::endCardScene() {
	auto back = HelloWorld::createScene();
	auto animation = TransitionFade::create(0.7f, back);
	Director::getInstance()->replaceScene(animation);
}

void CardScene::startNewRound() {
	if (roundNum < 3)
		roundNum++;
	// Clear All Cards On Board
	for (auto& it : lineCardNum) {
		it = 0;
	}
	// Show Card Trashing Animation
	for (auto& oppoC : oppoBoardCards) {
		auto animation = Sequence::create(MoveTo::create(1.0f, oppoTrash->getPosition()), FadeOut::create(0.3f), NULL);
		oppoC->runAction(Sequence::create(animation, CallFunc::create([oppoC, this] {
			oppoC->removeFromParentAndCleanup(true);
		}), nullptr));
	}
	oppoBoardCards.clear();
	for (auto& myC : myBoardCards) {
		auto animation = Sequence::create(MoveTo::create(1.0f, myTrash->getPosition()), FadeOut::create(0.3f), NULL);
		myC->runAction(Sequence::create(animation, CallFunc::create([myC, this] {
			myC->removeFromParentAndCleanup(true);
		}), nullptr));
	}
	myBoardCards.clear();
	// Clean Points and Label Values
	myPoints = 0;
	oppoPoints = 0;
	for (auto& itt : MyLinePoints) {
		itt = 0;
	}
	for (auto& sp : allLabels) {
		sp->setTexture(Director::getInstance()->getTextureCache()->addImage("characters/Numbers/number0.png"));
	}
	default0->setTexture(Director::getInstance()->getTextureCache()->addImage("default.png"));

	auto upMove = Sequence::create(DelayTime::create(0.5), Show::create(),
		OrbitCamera::create(0.5, 1.5, 0, 180, 90, 0, 0), NULL);
	auto downMove = Sequence::create(OrbitCamera::create(0.5, 1.5, 0, 0, 90, 0, 0), Hide::create(),
		DelayTime::create(0.5), NULL);

	if (myOriginFirst) {
		coinState = true;
		changeBoardState(true);
		bCoin->runAction(upMove);
		rCoin->runAction(downMove);
	}
	else {
		coinState = false;
		changeBoardState(false);
		bCoin->runAction(downMove);
		rCoin->runAction(upMove);
	}
	nowRoundState = true;
	oppoRoundState = true;
	if (handCardsNum == 0) {
		nowRoundState = false;
		access0.EndRound(sessionKey);
	}
}

void CardScene::initMyCards(vector<string> res) {
	handCardsNum = 13;
	// Init User's Cards
	for (int i = 1; i < res.size(); i++) {
		int id = Value(res[i]).asInt();
		auto result = getCardByID(id);
		string path0 = "characters/Decks/" + result->type + "/" + result->name + ".png";
		auto temp = Sprite::create(path0);
		cards.push_back(pair<int, Sprite*>(i - 1, temp));
		cardNames.push_back(pair<int, string>(i - 1, result->name));
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

	if (nowMsg != newMsg) {
		nowMsg = newMsg;

		// Variables Definitions
		auto upMove = Sequence::create(DelayTime::create(0.5), Show::create(),
			OrbitCamera::create(0.5, 1.5, 0, 180, 90, 0, 0), NULL);
		auto downMove = Sequence::create(OrbitCamera::create(0.5, 1.5, 0, 0, 90, 0, 0), Hide::create(),
			DelayTime::create(0.5), NULL);
		// Test Animation
		auto endAnimate = Sequence::create(Show::create(),
			ScaleTo::create(0.2, 0.6), DelayTime::create(0.8), Hide::create(), ScaleTo::create(0.1, 0.5), NULL);
		auto root = Director::getInstance()->getRunningScene();

		auto res = access0.split(nowMsg, "||");
		if (res[0] == "deck") {
			initMyCards(res);
			access0.GetCurrentStatus(sessionKey);
		}
		else if (res[0] == "status") {
			int win1 = Value(res[4]).asInt(), win2 = Value(res[5]).asInt();
			int deck1 = Value(res[6]).asInt(), deck2 = Value(res[7]).asInt();
			if (Value(res[4]).asInt() == 0 && Value(res[5]).asInt() == 0) {
				if (res[1] != myName) {
					coinState = false;
					myOriginFirst = false;
					changeBoardState(false);
					bCoin->setVisible(false);
					rCoin->setVisible(true);
				}
				else {
					showMyTurnSign();
				}
			}
		}
		else if (res.size() == 2) {
			if (res[0] == "opPlay") {
				addOppoCard(Value(res[1]).asInt());
				if (nowRoundState) {
					myTurnSign->runAction(endAnimate);
					coinState = true;
					changeBoardState(true);
					bCoin->runAction(upMove);
					rCoin->runAction(downMove);
				}
			}
			else if (res[0] == "play" && oppoRoundState == false) {
				showMyTurnSign();
			}
			else if (res[0] == "winner") {
				int temp = -2;
				if (res[1] == myName) {
					temp = 1;
				}
				else if (res[1] == oppoName) {
					temp = -1;
				}
				else {
					temp = 0;
				}
				judgeAndShow(temp);
			}
		}
		else if (res.size() == 1) {
			if (res[0] == "opEndRound") {
				oppoRoundState = false;
				auto oppoEndSign = Sprite::create("redSign.png");
				auto oppoEndLabel = Label::create("Your opponent: " + oppoName + " has ended this round!", "fonts/Marker Felt.ttf", 35);
				oppoEndSign->setPosition(visibleSize / 2);
				oppoEndLabel->setPosition(oppoEndSign->getContentSize() / 2);
				oppoEndSign->addChild(oppoEndLabel, 2);
				this->addChild(oppoEndSign);
				oppoEndSign->runAction(Sequence::create(endAnimate, CallFunc::create([oppoEndSign, this] {
					oppoEndSign->removeFromParentAndCleanup(true);
				}), nullptr));
				if(nowRoundState == true){
					myTurnSign->runAction(endAnimate);
					coinState = true;
					changeBoardState(true);
					bCoin->runAction(upMove);
					rCoin->runAction(downMove);
				}
			}
			else if (res[0] == "goNextRound") {
				allEndThisRound();
				startNewRound();
			}
		}
	}
}

void CardScene::judgeAndShow(int flag) {
	string endingMess = "";
	auto victory = Sprite::create("victory.png");
	auto defeat = Sprite::create("defeat.png");
	auto tie = Sprite::create("tie.png");
	auto endAnimate = Sequence::create(Show::create(),
		ScaleTo::create(0.3, 0.8), DelayTime::create(1.5), Hide::create(), NULL);
	auto endAndJump = Sequence::create(endAnimate, CallFunc::create([this] {
		this->setVisible(false);
		endCardScene();
	}), nullptr);
	myAudio->stopBackgroundMusic();

	switch (flag) {
		case 1: // My Wining
			victory->setScale(0.5);
			victory->setPosition(visibleSize / 2);
			this->addChild(victory, 3);
			victory->runAction(endAndJump);
			myAudio->playEffect("music/victory.mp3");
			break;
		case 0: // Draw
			tie->setScale(0.5);
			tie->setPosition(visibleSize / 2);
			this->addChild(tie, 3);
			tie->runAction(endAndJump);
			break;
		case -1: // My Failure
			defeat->setScale(0.5);
			defeat->setPosition(visibleSize / 2);
			this->addChild(defeat, 3);
			defeat->runAction(endAndJump);
			myAudio->playEffect("music/defeat.mp3");

	}
}

void CardScene::allEndThisRound() {
	// Show Round ending and decide who wins
	// Init Points and cards
	nowRoundState = true;
	oppoRoundState = true;
	string roundMess = "";
	string roundNumber = Value(roundNum).asString();
	string points = Value(myPoints).asString() + " : " + Value(oppoPoints).asString();
	if (myPoints > oppoPoints) {
		roundMess = "Round " + roundNumber + " End! You Win";
	}
	else if (myPoints == oppoPoints) {
		roundMess = "Round " + roundNumber + " End! It's A Tie";
	}
	else {
		roundMess = "Round " + roundNumber + " End! You Fail";
	}
	// Show Labels And Points
	auto showLabel = Label::createWithTTF(roundMess, "fonts/Marker Felt.ttf", 30);
	auto showPoints = Label::createWithTTF(points, "fonts/Marker Felt.ttf", 40);
	auto showBox = Sprite::create("pointsShow.png");
	auto blueStar = Sprite::create("blueStar.png");
	blueStar->setScale(0.4);
	auto redStar = Sprite::create("redStar.png");
	redStar->setScale(0.4);

	showLabel->setPosition(showBox->getContentSize() / 2);
	blueStar->setPosition(Vec2(showBox->getContentSize().width / 4, showBox->getContentSize().height / 2 - 80));
	redStar->setPosition(Vec2(showBox->getContentSize().width / 2 + 150, showBox->getContentSize().height / 2 - 80));
	showPoints->setPosition(Vec2(showBox->getContentSize().width / 2, blueStar->getPosition().y));
	showBox->addChild(showLabel, 1);
	showBox->addChild(blueStar, 1);
	showBox->addChild(redStar, 1);
	showBox->addChild(showPoints, 1);
	showBox->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 20));
	showBox->setScale(0.6);
	this->addChild(showBox, 3);

	auto showPointsAnimate = Sequence::create(Show::create(),
		ScaleTo::create(0.2, 0.6), DelayTime::create(2.0), Hide::create(), NULL);
	showBox->runAction(Sequence::create(showPointsAnimate, CallFunc::create([showBox, this] {
		showBox->removeFromParentAndCleanup(true);
	}), nullptr));

}

void CardScene::showMyTurnSign() {
	auto showAnimate = Sequence::create(Show::create(),
		ScaleTo::create(0.2, 0.6), DelayTime::create(0.8), Hide::create(), ScaleTo::create(0.1, 0.5), NULL);
	myTurnSign->runAction(showAnimate);
}

void CardScene::initLines() {
	int order_height = 85;
	// Add My Lines
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
	// Add Opponent Lines
	order_height = visibleSize.height / 2 + 50;
	for (int i = 3; i < 6; i++) {
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

void CardScene::addOppoCard(int index) {
	auto root = Director::getInstance()->getRunningScene();
	int oppoCardSize = opponentCards.size();
	auto tt = opponentCards[oppoCardSize - 1];
	opponentCards.pop_back();
	tt->removeFromParentAndCleanup(true);
	Card* target = getCardByID(index);
	// Create show image
	string path0 = "characters/Decks/" + target->type + "/" + target->name + ".png";
	auto showTarget = Sprite::create(path0);
	showTarget->setScale(0.17);
	vector<string> type = { "normal", "epic", "exotic" };
	int count = 3;
	for (auto e : type) {
		if (e == target->type)
			break;
		else
			count++;
	}
	auto theLine = root->getChildByName("line" + Value(count).asString());
	showTarget->setPosition(Vec2(visibleSize.width / 2 - 250 + lineCardNum[count] * 60, theLine->getPosition().y));
	this->addChild(showTarget, 1);
	lineCardNum[count]++;
	oppoBoardCards.push_back(showTarget);
	changePoints(count, target->attack);
}

void CardScene::initPointLabels() {
	myPoints = oppoPoints = 0;
	for (int i = 0; i < 6; i++) {
		MyLinePoints.push_back(0);
	}
	// My point labels
	int order_height = visibleSize.height / 2 - 45;
	for (int i = 0; i < 3; i++) {
		auto num1 = Sprite::create("characters/Numbers/number0.png");
		auto num2 = Sprite::create("characters/Numbers/number0.png");
		num1->setPosition(Vec2(143, order_height));
		num2->setPosition(Vec2(163, order_height));
		num1->setScale(0.6);
		num2->setScale(0.6);
		this->addChild(num1, 1);
		this->addChild(num2, 1);
		allLabels.push_back(num1);
		allLabels.push_back(num2);
		order_height -= 75;
	}
	// My Total labels
	auto num1 = Sprite::create("characters/Numbers/number0.png");
	auto num2 = Sprite::create("characters/Numbers/number0.png");
	auto num3 = Sprite::create("characters/Numbers/number0.png");
	num1->setPosition(Vec2(32, visibleSize.height / 2 - 34));
	num2->setPosition(Vec2(52, visibleSize.height / 2 - 34));
	num3->setPosition(Vec2(72, visibleSize.height / 2 - 34));
	num1->setScale(0.6);
	num2->setScale(0.6);
	num3->setScale(0.6);
	this->addChild(num1, 1);
	this->addChild(num2, 1);
	this->addChild(num3, 1);
	allLabels.push_back(num1);
	allLabels.push_back(num2);
	allLabels.push_back(num3);

	// Opponent point Labels
	order_height = visibleSize.height / 2 + 60;
	for (int i = 0; i < 3; i++) {
		auto num1 = Sprite::create("characters/Numbers/number0.png");
		auto num2 = Sprite::create("characters/Numbers/number0.png");
		num1->setPosition(Vec2(143, order_height));
		num2->setPosition(Vec2(163, order_height));
		num1->setScale(0.6);
		num2->setScale(0.6);
		this->addChild(num1, 1);
		this->addChild(num2, 1);
		allLabels.push_back(num1);
		allLabels.push_back(num2);
		order_height += 75;
	}
	// Opponent total labels
	// My Total labels
	auto num4 = Sprite::create("characters/Numbers/number0.png");
	auto num5 = Sprite::create("characters/Numbers/number0.png");
	auto num6 = Sprite::create("characters/Numbers/number0.png");
	num4->setPosition(Vec2(32, visibleSize.height / 2 + 87));
	num5->setPosition(Vec2(52, visibleSize.height / 2 + 87));
	num6->setPosition(Vec2(72, visibleSize.height / 2 + 87));
	num4->setScale(0.6);
	num5->setScale(0.6);
	num6->setScale(0.6);
	this->addChild(num4, 1);
	this->addChild(num5, 1);
	this->addChild(num6, 1);
	allLabels.push_back(num4);
	allLabels.push_back(num5);
	allLabels.push_back(num6);
}

void CardScene::initGameDatas() {
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
	if (cards.size() == 0)
		return false;
	for (int i = 0; i < 13; i++) {
		Sprite* temp = (Sprite*)root->getChildByTag(i);
		if (!temp)
			continue;
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
	if (cards.size() == 0)
		return;
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
	string name = cardNames[target].second;
	Card* theCard = getCardByName(name);
	vector<string> type = {"exotic","epic", "normal"};
	if (onBoard[target] == true) {
		cardPos->setPosition(originPos[target]);
		return;
	}
	for (int i = 0; i < 3; i++) {
		auto temp = (Sprite*)root->getChildByName("line" + Value(i).asString());
		auto judge_height = 70;
		if (i == 2)
			judge_height = 50;
		else if (i == 0)
			judge_height = 100;
		if (temp->getBoundingBox().containsPoint(cardPos->getPosition()) && theCard->type == type[i]) {
			playOutACard(target);
			cardPos->setPosition(Vec2(visibleSize.width / 2 - 250 + lineCardNum[i]*60, judge_height + 100*i));
			onBoard[target] = true;
			originPos[target] = cardPos->getPosition();
			correctFlag = true;
			lineCardNum[i]++;
			myBoardCards.push_back(cardPos);
			changePoints(2-i, theCard->attack);
			auto upMove = Sequence::create(DelayTime::create(0.5), Show::create(),
				OrbitCamera::create(0.5, 1.5, 0, 180, 90, 0, 0), NULL);
			auto downMove = Sequence::create(OrbitCamera::create(0.5, 1.5, 0, 0, 90, 0, 0), Hide::create(),
				DelayTime::create(0.5), NULL);

			if (oppoRoundState == true) {
				bCoin->runAction(downMove);
				rCoin->runAction(upMove);
				changeBoardState(false);
				coinState = false;
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

void CardScene::playOutACard(int i) {
	handCardsNum--;
	// Play effect
	myAudio->playEffect("music/kick.mp3", false);
	if (handCardsNum == 0) {
		auto upMove = Sequence::create(DelayTime::create(0.5), Show::create(),
			OrbitCamera::create(0.5, 1.5, 0, 180, 90, 0, 0), NULL);
		auto downMove = Sequence::create(OrbitCamera::create(0.5, 1.5, 0, 0, 90, 0, 0), Hide::create(),
			DelayTime::create(0.5), NULL);

		nowRoundState = false;
		access0.EndRound(sessionKey);
		coinState = false;
		changeBoardState(false);
		bCoin->runAction(downMove);
		rCoin->runAction(upMove);
		// Test Animation
		auto endAnimate = Sequence::create(Show::create(),
			ScaleTo::create(0.2, 0.6), DelayTime::create(0.8), Hide::create(), ScaleTo::create(0.1, 0.5), NULL);
		endRoundSign->runAction(endAnimate);
	}
	auto target = cardNames[i].second;
	auto theCard = getCardByName(target);
	access0.PlayOutaCard(sessionKey, theCard->index);
}

void CardScene::changePoints(int row, int point) {
	MyLinePoints[row] += point;
	int temp = 0;
	int upOrDown = 0, factor = 0;
	if (row >= 3) {
		upOrDown = 1;
		oppoPoints += point;
		temp = oppoPoints;
		factor = 3;
	}
	else {
		myPoints += point;
		temp = myPoints;
	}
	if (MyLinePoints[row] < 10) {
		allLabels[row * 2 + 1 + factor]->setTexture(Director::getInstance()->getTextureCache()->addImage("characters/Numbers/number" + Value(MyLinePoints[row]).asString() + ".png"));
	}
	else {
		int first = MyLinePoints[row] / 10, second = MyLinePoints[row] % 10;
		allLabels[row * 2 + factor]->setTexture(Director::getInstance()->getTextureCache()->addImage("characters/Numbers/number" + Value(first).asString() + ".png"));
		allLabels[row * 2 + 1 + factor]->setTexture(Director::getInstance()->getTextureCache()->addImage("characters/Numbers/number" + Value(second).asString() + ".png"));
	}
	if (temp < 10) {
		allLabels[upOrDown * 9 + 8]->setTexture(Director::getInstance()->getTextureCache()->addImage("characters/Numbers/number" + Value(temp).asString() + ".png"));
	}
	else if(temp < 100){
		int first = temp / 10, second = temp % 10;
		allLabels[upOrDown * 9 + 7]->setTexture(Director::getInstance()->getTextureCache()->addImage("characters/Numbers/number" + Value(first).asString() + ".png"));
		allLabels[upOrDown * 9 + 8]->setTexture(Director::getInstance()->getTextureCache()->addImage("characters/Numbers/number" + Value(second).asString() + ".png"));
	}
	else {
		int first = temp / 100, second = (temp - 100 * first) / 10, third = temp % 10;
		allLabels[upOrDown * 9 + 6]->setTexture(Director::getInstance()->getTextureCache()->addImage("characters/Numbers/number" + Value(first).asString() + ".png"));
		allLabels[upOrDown * 9 + 7]->setTexture(Director::getInstance()->getTextureCache()->addImage("characters/Numbers/number" + Value(second).asString() + ".png"));
		allLabels[upOrDown * 9 + 8]->setTexture(Director::getInstance()->getTextureCache()->addImage("characters/Numbers/number" + Value(third).asString() + ".png"));
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
	if (coinState == false)
		return;
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
	if (cards.size() == 13) {
		for (int i = 0; i < 13; i++) {
			Sprite* sp = (Sprite*)root->getChildByTag(i);
			if (sp == NULL)
				continue;
			if (sp->getPosition().getDistance(pos) <= 40) {
				if (!outFlag[i]) {
					outFlag[i] = true;
					auto animate = ScaleTo::create(0.5f, 0.20f, 0.20f);
					sp->runAction(animate);
				}
			}
			else {
				if (outFlag[i]) {
					auto animate = ScaleTo::create(0.5f, 0.17f, 0.17f);
					sp->runAction(animate);
				}
				outFlag[i] = false;
			}
		}
	}
	if (opponentCards.size() == 13) {
		for (int i = 0; i < 13; i++) {
			auto oppoCard = opponentCards[i];
			if (oppoCard->getPosition().getDistance(pos) <= 40) {
				if (!OppOutFlag[i]) {
					OppOutFlag[i] = true;
					auto animate = ScaleTo::create(0.5f, 0.24f, 0.24f);
					oppoCard->runAction(animate);
				}
			}
			else {
				if (OppOutFlag[i]) {
					auto animate = ScaleTo::create(0.5f, 0.21f, 0.21f);
					oppoCard->runAction(animate);
				}
				OppOutFlag[i] = false;
			}
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
	if (cards.size() == 0)
		return;
	for (int i = 0; i < 13; i++) {
		Sprite* sp = (Sprite*)root->getChildByTag(i);
		if (sp == NULL)
			continue;
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
