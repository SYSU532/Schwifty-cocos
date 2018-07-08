#include "CardScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* CardScene::createScene()
{
	return CardScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool CardScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	Director::getInstance()->getOpenGLView()->setFrameSize(1748, 1012);
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();
	
	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	
	auto bg = Sprite::create("board.png");
	bg->setPosition(Vec2(visibleSize.width / 2, visibleSize.height /2 + 20));
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

	initCards();
	initAndCleanClick();
	initLines();
	
	return true;
}

void CardScene::initCards() {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	// Init User's Cards
	for (int i = 0; i < 13; i++) {
		auto temp = Sprite::create("Morty1.png");
		cards.pushBack(temp);
	}


	// Show cards
	Vector<Sprite*>::iterator it = cards.begin();
	int borderWidth = 70;
	int i = 0;
	for (; it != cards.end(); it++) {
		(*it)->setScale(0.18);
		(*it)->setPosition(Vec2(borderWidth, 20));
		(*it)->setTag(i);
		this->addChild((*it), 1);
		borderWidth += 68;
		originPos.push_back((*it)->getPosition());
		i++;
	}

	auto test = Sprite::create("characters/Decks/common/Morty.png");
	test->setPosition(Vec2(visibleSize.width - 120, visibleSize.height - 120));
	test->setScale(0.5);
	this->addChild(test, 1);

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
		}
	}
	if (!correctFlag) {
		cardPos->setPosition(originPos[target]);
	}
	initAndCleanClick();
	for (auto& e : outFlag) {
		e = false;
	}

}

void CardScene::onTouchMoved(Touch *touch, Event *event) {
	for (int i = 0; i < 13; i++) {
		if (isClick[i]) {
			auto delta = touch->getDelta();
			Sprite* temp = NULL;
			Vector<Sprite*>::iterator it = cards.begin();
			for (int j = 0; it != cards.end(); j++, it++) {
				if (j == i) {
					temp = *it;
				}
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
				auto animate = ScaleTo::create(1.0f, 0.21f, 0.21f);
				sp->runAction(animate);
			}
		}
		else {
			if (outFlag[i]) {
				auto animate = ScaleTo::create(1.0f, 0.18f, 0.18f);
				sp->runAction(animate);
			}
			outFlag[i] = false;
		}
	}
}