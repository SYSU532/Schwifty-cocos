#include "StartMenu.h"
#include "HelloWorldScene.h"
#include "MyDialog.h"
#include "PickPage.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;


Scene* StartMenu::createScene()
{
	return StartMenu::create();
}

bool StartMenu::init()
{
	
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	auto bg = Sprite::create("bgg.jpg");
	bg->setPosition(visibleSize / 2);
	bg->setScale(visibleSize.width / bg->getContentSize().width,
		visibleSize.height / bg->getContentSize().height);
	this->addChild(bg, 0);

	auto LoginBtn = MenuItemImage::create("start.png", "start2.png", CC_CALLBACK_1(StartMenu::signin, this));
	LoginBtn->setScale(0.6);
	auto LogupBtn = MenuItemImage::create("register1.png", "register.png", CC_CALLBACK_1(StartMenu::signup, this));
	LogupBtn->setScale(0.6);
	if (LoginBtn && LogupBtn) {
		float x = origin.x + visibleSize.width / 2;
		float y1 = origin.y + LoginBtn->getContentSize().height / 2 + 250;
		float y2 = origin.y + LogupBtn->getContentSize().height / 2 + 150;
		LoginBtn->setPosition(x + 200, y1);
		LogupBtn->setPosition(x + 200, y2);
	}

	auto menu = Menu::create(LoginBtn, LogupBtn, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	schedule(schedule_selector(StartMenu::LogUpdate), 0.05f, kRepeatForever, 0);

	nowMsg = "";

	playBgm();

	

	return true;
}

void StartMenu::signin(Ref* p) {
	// Handle NetWork Signin
	MyDialog* d1 = MyDialog::create();
	d1->chooseMode(0);
	d1->setTitle("Sign In");
	d1->setEnterCallBackFunc(this, callfuncN_selector(StartMenu::onLoginAdmit));
	d1->setName("Dialog");
	this->addChild(d1, 1);
}

void StartMenu::LogUpdate(float f) {
	string newMsg = access0.getMessage();
	auto root = Director::getInstance()->getRunningScene();
	if (nowMsg != newMsg) {
		nowMsg = newMsg;
		auto spRes = access0.split(nowMsg, "||");
		if (spRes.size() == 2) {
			auto res = access0.ParseLogin(nowMsg);
			auto code = res.first;
			auto msg = res.second;
			MyDialog* dd = (MyDialog*)root->getChildByName("Dialog");
			if (dd == NULL) {
				dd = (MyDialog*)root->getChildByName("RDialog");
				if (dd == NULL)
					return;
			}
			if (code == false) {
				dd->changeMsg(msg, 0);
			}
			else {
				dd->changeMsg("OK", 1);
				UserDefault::getInstance()->setStringForKey("userkey", msg);
				dd->removeFromParentAndCleanup(true);
				pickRick();
			}
		}
	}
}

void StartMenu::onLoginAdmit(Node* pNode) {
	auto username = UserDefault::getInstance()->getStringForKey("username");
	auto password = UserDefault::getInstance()->getStringForKey("password");
	access0.AttemptLogin(username, password);
}

void StartMenu::signup(Ref* p) {
	MyDialog* d2 = MyDialog::create();
	d2->chooseMode(1);
	d2->setTitle("Regist Account");
	d2->setEnterCallBackFunc(this, callfuncN_selector(StartMenu::onLogupAdmit));
	d2->setName("RDialog");
	this->addChild(d2, 1);
}

void StartMenu::onLogupAdmit(Node* pNode) {
	auto username = UserDefault::getInstance()->getStringForKey("username");
	auto password = UserDefault::getInstance()->getStringForKey("password");
	auto email = UserDefault::getInstance()->getStringForKey("email");
	HttpRequest* request = new HttpRequest();
	auto phone = random() * 1000;
	std::string postData = "name=" + username + "&pass=" + password
		+ "&email=" + email + "&imgType=.jpg" + "&phone=" + Value(phone).asString() + "&default=1&rePass=" + password;
	request->setUrl("https://esblog.chenmt.science/logup?" + postData);
	request->setRequestType(HttpRequest::Type::POST);
	request->setResponseCallback(CC_CALLBACK_2(StartMenu::onLogupResponse, this));

	
	CCLOG(postData.c_str());
	request->setRequestData(postData.c_str(), postData.length());
	HttpClient::getInstance()->send(request);
	request->release();
}

void StartMenu::onLogupResponse(HttpClient* sender, HttpResponse* res) {
	if (res) {
		if (res->isSucceed()) {
			CCLOG("success");
			rapidjson::Document doc;
			auto buff = res->getResponseData();
			doc.Parse(buff->data(), buff->size());
			auto msg = doc["errMessage"].GetString();
			auto code = doc["code"].GetInt();
			auto root = Director::getInstance()->getRunningScene();
			MyDialog* dd = (MyDialog*)root->getChildByName("RDialog");
			if (code == 0) {
				std::string m1 = "", m2 = "";
				if (Value(msg).asString().size() > 25) {
					m1 = Value(msg).asString().substr(0, 21);
					m2 = Value(msg).asString().substr(21, Value(msg).asString().length());
				}
				else {
					m1 = msg;
				}
				auto m = m1 + "\n" + m2;
				dd->changeMsg(m, 0);
			}
			else {
				auto username = UserDefault::getInstance()->getStringForKey("username");
				auto password = UserDefault::getInstance()->getStringForKey("password");
				access0.AttemptLogin(username, password);
				dd->removeFromParentAndCleanup(true);
				pickRick();
			}
		}
	}
}

void StartMenu::pickRick() {
	Scene* s = NULL;
	auto username = UserDefault::getInstance()->getStringForKey("username");
	auto has = UserDefault::getInstance()->getIntegerForKey((username + "_head").c_str());
	if (has != NULL) {
		s = HelloWorld::createScene();
	}
	else {
		s = PickPage::createScene();
	}
	auto animate = TransitionFade::create(0.5f, s);
	CCDirector::sharedDirector()->replaceScene(animate);
}

void StartMenu::playBgm() {
	auto myAudio = SimpleAudioEngine::getInstance();
	myAudio->preloadBackgroundMusic("music/get_schwifty.mp3");
	myAudio->playBackgroundMusic("music/get_schwifty.mp3", true);

}