#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "CardScene.h"

USING_NS_CC;
using namespace CocosDenshion;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
	Director::getInstance()->getOpenGLView()->setFrameSize(1060, 704);

	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();
    
	username = UserDefault::getInstance()->getStringForKey("username");
	std::string head = username + "_head";
	auto userRick = UserDefault::getInstance()->getIntegerForKey(head.c_str()) - 1;
	std::string headPath = "characters/" + Value(userRick).asString() + "/Rick4.png";
	path = "characters/" + Value(userRick).asString();

	player = Sprite::create(headPath);
	player->setPosition(visibleSize.width / 3, visibleSize.height / 4 + 100);
	player->setScale(0.3);
	this->addChild(player, 1);
	placeTileMap();

	book = Sprite::create("book.png");
	book->setPosition(Vec2(visibleSize.width  - 110, visibleSize.height + 20));
	book->setScale(0.35);
	this->addChild(book, 1);

	auto winSize = Director::sharedDirector()->getWinSize();
	cam = Camera::createPerspective(60.0, (float)winSize.width / winSize.height, 1, 1000);
	cam->setRotation3D(Vec3(-45, 0, 0));
	this->addChild(cam);
	
	initUserInfo();

	addTouchListener();
	addMouseListener();
	addKeyBoardListener();

	preLoadMusic();
	playBgm();

	schedule(schedule_selector(HelloWorld::update), 0.05f, kRepeatForever, 0);

	state = 0;
	outFlag = false;

    return true;
}

void HelloWorld::update(float f) {
	if (isMove) {
		move(movekey);

	}
	cam->setPosition3D(player->getPosition3D());

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
	userLevel = 10;
	userCardsNum = 13;
	userHeadImgType = 0;
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

void HelloWorld::attack() {
	Scene* s = CardScene::createScene();
	auto animate = TransitionFade::create(1.5f, s);
	CCDirector::sharedDirector()->replaceScene(animate);
	auto audio = SimpleAudioEngine::getInstance();
	audio->stopBackgroundMusic();
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
	if (dest.x < 0)
		dest.x = 0;
	else if (dest.x > visibleSize.width)
		dest.x = visibleSize.width;
	if (dest.y < 0)
		dest.y = 0;
	else if (dest.y > visibleSize.height)
		dest.y = visibleSize.height;
		
	auto move = MoveTo::create(0.1f, dest);
	auto animation = Animation::createWithSpriteFrames(Move, 0.2f);
	auto animate = Animate::create(animation);

	auto repeat = Repeat::create(animate, 1);
	player->runAction(repeat);
	player->runAction(move);
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

void HelloWorld::onTouchMoved(Touch* touchs, Event* event) {
	
}

bool HelloWorld::onTouchBegin(Touch* touch, Event* event) {
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
		attack();
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


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
