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

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();
	Director::getInstance()->getOpenGLView()->setFrameSize(1948, 1036);
	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	
	auto bg = Sprite::create("board.png");
	bg->setPosition(visibleSize / 2);
	bg->setScale(visibleSize.width / bg->getContentSize().width,
		visibleSize.height / bg->getContentSize().height);
	this->addChild(bg, 0);

	initCards();
	
	return true;
}

void CardScene::initCards() {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	// Init User's Cards
	for (int i = 0; i < 13; i++) {
		auto temp = Sprite::create("Morty1.png");
		temp->setScale(0.5);
		cards.pushBack(temp);
	}


	// Show cards
	Vector<Sprite*>::iterator it = cards.begin();
	int borderWidth = 150;
	for (; it != cards.end(); it++) {
		(*it)->setScale(0.3);
		(*it)->setPosition(Vec2(borderWidth, 40));
		this->addChild((*it), 1);
		borderWidth += 130;
	}

}