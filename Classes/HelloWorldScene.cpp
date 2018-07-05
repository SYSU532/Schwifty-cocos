#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

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

	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();
    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
	auto clickLabel = Label::createWithTTF("Click Me", "fonts/Marker Felt.ttf", 25);
	clickLabel->enableOutline(Color4B::ORANGE, 3);
	//auto clickItem = MenuItemLabel::create(clickLabel, CC_CALLBACK_1(HelloWorld::menuClickCallback, this));
	

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
		//clickItem->setPosition(Vec2(x - 30, y + 50));
    }

    // create menu, it's an autorelease object
    auto menu1 = Menu::create(closeItem, NULL);
	//auto menu2 = Menu::create(clickItem, NULL);
    menu1->setPosition(Vec2::ZERO);
	//menu2->setPosition(Vec2::ZERO);
    this->addChild(menu1, 1);
	//this->addChild(menu2, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
	auto back = Sprite::create("img/bg.png");
	back->setPosition(visibleSize / 2);
	back->setScale(visibleSize.width / back->getContentSize().width,
		visibleSize.height / back->getContentSize().height);
	this->addChild(back, 0);
	player = Sprite::create("Rick12.png");
	player->setPosition(visibleSize.width / 2, player->getContentSize().height);
	this->addChild(player, 1);
	//placeTileMap();

	auto winSize = Director::sharedDirector()->getWinSize();
	cam = Camera::create();
	this->addChild(cam);
	
	addTouchListener();
	addKeyBoardListener();

	schedule(schedule_selector(HelloWorld::update), 0.05f, kRepeatForever, 0);

    return true;
}

void HelloWorld::update(float f) {
	if (isMove)
		move(movekey);
	// Other Events

}

void HelloWorld::attack() {
	Vector<SpriteFrame*> Attack;
	Attack.reserve(8);
	char attackPath[20];
	for (int i = 0; i < 8; i++) {
		sprintf(attackPath, "img/attack%d.png", i);
		auto frame = SpriteFrame::create(attackPath, Rect(0, 0, 300, 300));
		Attack.pushBack(frame);
	}
	auto animation = Animation::createWithSpriteFrames(Attack, 0.1f);
	auto animate = Animate::create(animation);
	player->stopAllActions();
	player->runAction(animate);
}

void HelloWorld::move(char dir) {
	Vector<SpriteFrame*> Move;
	Move.reserve(6);
	char movePath[20];
	for (int i = 12; i < 16; i++) {
		sprintf(movePath, "Rick%d.png", i);
		auto frame = SpriteFrame::create(movePath, Rect(0, 0, 125, 165));
		Move.pushBack(frame);
	}

	auto offset = Vec2(0, 0);
	switch (dir) {
		case 'W':
			offset.y = 30;
			break;
		case 'A':
			offset.x = -30;
			break;
		case 'S':
			offset.y = -30;
			break;
		case 'D':
			offset.x = 30;
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
	auto move = MoveTo::create(0.5f, dest);
	auto animation = Animation::createWithSpriteFrames(Move, 0.2f);
	auto animate = Animate::create(animation);

	auto repeat = RepeatForever::create(animate);
	player->runAction(repeat);
	player->runAction(move);
}

void HelloWorld::placeTileMap() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	TMXTiledMap* map = TMXTiledMap::create("map.tmx");
	map->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	map->setAnchorPoint(Vec2(0.5, 0.5));
	map->setScale(Director::getInstance()->getContentScaleFactor());
	this->addChild(map, 0);
}

void HelloWorld::addTouchListener() {
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegin, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void HelloWorld::addKeyBoardListener() {
	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPress, this);
	keyListener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyReleased, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyListener, this);
}

void HelloWorld::onTouchMoved(Touch* touchs, Event* event) {
	/*Point newPos = touchs->getDelta();
	Vec3 cameraPos = cam->getPosition3D();
	cameraPos.y -= newPos.y;
	cameraPos.x -= newPos.x;
	cam->setPosition3D(cameraPos);
	*/
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
