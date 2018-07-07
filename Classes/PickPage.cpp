#include "PickPage.h"
#include "HelloWorldScene.h"

USING_NS_CC;

Scene* PickPage::createScene() {
	return PickPage::create();
}

bool PickPage::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	Director::getInstance()->getOpenGLView()->setFrameSize(1060, 704);

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	username = UserDefault::getInstance()->getStringForKey("username");

	for (int i = 0; i < 6; i++) {
		std::string path =  "characters/BigImgs/Rick" + Value(i).asString() + ".png";
		auto temp = Sprite::create(path);
		rickArr.pushBack(temp);
	}

	Vector<Sprite*>::iterator it = rickArr.begin();
	int order_width = 105;
	for (; it != rickArr.end(); it++) {
		(*it)->setPosition(Vec2(order_width, visibleSize.height / 2));
		(*it)->setScale(0.4);
		order_width += 145;
		this->addChild(*it, 1);
	}

	auto title = Label::createWithTTF("Pick your favorite Rick !", "fonts/Marker Felt.ttf", 40);
	title->setColor(Color3B::ORANGE);
	title->setPosition(Vec2(visibleSize.width / 2 + 50, visibleSize.height - 50));
	this->addChild(title, 1);

	auto menu = Menu::create();
	rickName = {"Bald Rick", "Beard Rick", "BubbleGum Rick", "Normal Rick", "JunkYard Rick", "SuperFan Rick"};
	order_width = 110;
	for (int i = 0; i < rickName.size(); i++) {
		auto temp = MenuItemImage::create("btn0.jpg", "btn1.jpg", CC_CALLBACK_1(PickPage::onSubmit, this));
		temp->setName(rickName[i]);
		temp->setTag(i);
		auto name = LabelTTF::create(rickName[i], "fonts/Marker Felt.ttf", 15);
		name->setColor(Color3B::GRAY);
		name->setPosition(temp->getContentSize() / 2);
		temp->addChild(name, 1);
		temp->setPosition(Vec2(order_width, 150));
		order_width += 145;
		menu->addChild(temp, 1);
	}
	
	auto enter = MenuItemImage::create("btn0.jpg", "btn1.jpg", CC_CALLBACK_1(PickPage::onStart, this));
	enter->setPosition(Vec2(650, 30));
	auto name = LabelTTF::create("Start Game", "fonts/Marker Felt.ttf", 15);
	name->setColor(Color3B::GRAY);
	name->setPosition(enter->getContentSize() / 2);
	enter->addChild(name, 1);
	menu->addChild(enter, 1);

	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	choice = Label::createWithTTF("Your Choice is: ", "fonts/Marker Felt.ttf", 30);
	choice->setColor(Color3B::GRAY);
	choice->setPosition(Vec2(250, 30));
	this->addChild(choice, 1);

	return true;
}

void PickPage::onSubmit(Ref* p) {
	Node* target = (Node*)p;
	whichRick = target->getTag() + 1;
	string cc = "Your Choice is:  " + target->getName();
	choice->setString(cc);
}

void PickPage::onStart(Ref* p) {
	string head = username + "_head";
	UserDefault::getInstance()->setIntegerForKey(head.c_str(), whichRick);
	Scene* s = HelloWorld::createScene();
	auto animate = TransitionFade::create(1.0f, s);
	CCDirector::sharedDirector()->replaceScene(animate);
}

bool PickPage::onTouchBegan(Touch* t, Event* e) {
	return true;
}