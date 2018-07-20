#include "MagicBook.h"

Scene* MagicBook::createScene() {
	return MagicBook::create();
}

bool MagicBook::init() {
	if (!Scene::init())
	{
		return false;
	}
	Director::getInstance()->getOpenGLView()->setFrameSize(960, 580);

	auto visibleSize = Director::getInstance()->getVisibleSize();
	myName = UserDefault::getInstance()->getStringForKey("username");
	string headName = UserDefault::getInstance()->getStringForKey("username") + "_head";
	rickType = UserDefault::getInstance()->getIntegerForKey(headName.c_str()) - 1;
	string rickPath = "heads/Rick" + Value(rickType).asString() + ".png";

	auto bg = Sprite::create("bigBook.png");
	bg->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	bg->setScale(visibleSize.width / bg->getContentSize().width,
		visibleSize.height / bg->getContentSize().height);
	this->addChild(bg, 0);

	auto back = MenuItemImage::create("exit0.png", "exit1.png", CC_CALLBACK_1(MagicBook::backBtn, this));
	back->setPosition(Vec2(80, visibleSize.height - 30));
	back->setScale(0.6);

	auto menu0 = Menu::create(back, NULL);
	menu0->setPosition(Vec2::ZERO);
	this->addChild(menu0);

	// Adding Morty and Rick
	auto morty = Sprite::create("MortyHead.png");
	auto rick = Sprite::create(rickPath);
	morty->setScale(0.25);
	rick->setScale(0.2);
	morty->setPosition(Vec2(150, 30));
	rick->setPosition(Vec2(visibleSize.width - 150, 30));
	morty->setRotation(180);
	rick->setRotation(-180);
	rick->setFlipX(true);
	this->addChild(morty, 1);
	this->addChild(rick, 1);

	initJSONDetails();

	// Send NetWork Request
	access0.GetAllDecks(myName);
	// Set scheduler
	schedule(schedule_selector(MagicBook::networkUpdate), 0.02f, kRepeatForever, 0);

	// Adding title
	auto title = Sprite::create("bTitle.png");
	title->setScale(0.6);
	title->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 10));
	this->addChild(title, 1);

	// Load BGM
	auto myAudio = SimpleAudioEngine::getInstance();
	myAudio->preloadBackgroundMusic("music/memories.mp3");
	myAudio->playBackgroundMusic("music/memories.mp3", true);

	return true;
}

void MagicBook::backBtn(Ref* f) {
	this->setVisible(false);

	auto s = HelloWorld::createScene();
	auto transition = TransitionFade::create(0.6f, s);
	Director::getInstance()->replaceScene(transition);
}

void MagicBook::networkUpdate(float f) {
	string mess = access0.getMessage();
	auto res = access0.split(mess, "||");
	if (res.size() <= 2)
		return;

	if (res[0] == "allDeck" && res[1] == myName) {
		int order_height = 35, order_width = 90;
		for (int i = 2; i < res.size(); i++) {
			auto targetCard = getCardByID(Value(res[i]).asInt());
			string path = "characters/Decks/" + targetCard->type + "/" + targetCard->name + ".png";
			auto temp = Sprite::create(path);
			temp->setScale(0.24);
			if ((i - 2) % 6 == 0) {
				order_height += 110;
				order_width = 135;
			}
			else {
				order_width += 135;
			}
			temp->setPosition(Vec2(order_width, order_height));
			this->addChild(temp, 1);
		}
	}
	
}

void MagicBook::initJSONDetails() {
	string data = FileUtils::getInstance()->getStringFromFile("json/cardset.json");
	rapidjson::Document doc;
	doc.Parse(data.c_str());
	auto cardArr = doc.GetArray();
	for (auto& obj : cardArr) {
		auto store = new Card(obj["attack"].GetInt(), obj["type"].GetString(), obj["name"].GetString(), obj["index"].GetInt());
		jsonDetails.push_back(store);
	}
}

Card* MagicBook::getCardByID(int id) {
	for (auto obj : jsonDetails) {
		if (obj->index == id) {
			return obj;
		}
	}

}