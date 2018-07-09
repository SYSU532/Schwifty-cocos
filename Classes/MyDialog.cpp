#include "MyDialog.h"
#include "vector"
USING_NS_CC;



MyDialog::MyDialog() {
	m_menu = NULL;
	m_callbackListener = NULL;
	m_Entercallback = NULL;
	m_Cancelcallback = NULL;
	m_background = NULL;
	m_title = NULL;
}

MyDialog::~MyDialog() {
	CC_SAFE_RELEASE(m_menu);
	CC_SAFE_RELEASE(m_background);
	CC_SAFE_RELEASE(m_title);
}

bool MyDialog::onTouchBegan(Touch* t, Event* e) {
	return true;
}

bool MyDialog::init() {
	if (!Layer::init()) {
		return false;
	}
	this->setContentSize(Size::ZERO);
	Menu* menu = Menu::create();
	menu->setPosition(0, 0);
	setMenuButton(menu);
	setSpriteBackGround(Sprite::create("dialog.png"));
	// Adding Enter and Cancel Buttons
	MenuItemImage* Enter = MenuItemImage::create("btn0.jpg", "btn1.jpg", this, menu_selector(MyDialog::EnbtnCallBack));
	MenuItemImage* Cancel = MenuItemImage::create("btn0.jpg", "btn1.jpg", this, menu_selector(MyDialog::CabtnCallBack));
	auto winSize = Director::getInstance()->getVisibleSize();
	Enter->setPosition(winSize / 2);
	Cancel->setPosition(winSize / 2);
	auto size1 = Enter->getContentSize();
	auto size2 = Cancel->getContentSize();
	auto enter = LabelTTF::create("Enter", "arial", 30);
	auto cancel = LabelTTF::create("Cancel", "arial", 30);
	enter->setPosition(size1 / 2);
	enter->setColor(Color3B::BLACK);
	Enter->setName("Enter");
	cancel->setPosition(size2 / 2);
	cancel->setColor(Color3B::BLACK);
	Cancel->setName("Cancel");
	Enter->addChild(enter);
	Cancel->addChild(cancel);

	getMenuButton()->addChild(Enter, 1);
	getMenuButton()->addChild(Cancel, 1);

	setTouchEnabled(true);

	return true;
}

static MyDialog* create(const char* bgImage) {
	MyDialog* temp = MyDialog::create();
	temp->setSpriteBackGround(Sprite::create(bgImage));
	return temp;
}

void MyDialog::setTitle(const char* title, int fontSize) {
	LabelTTF* title1 = LabelTTF::create(title, "fonts/arial.ttf", fontSize);
	setLabelTitle(title1);
}

void MyDialog::setEnterCallBackFunc(CCObject* target, SEL_CallFuncN callback) {
	m_callbackListener = target;
	m_Entercallback = callback;
}

void MyDialog::setCancelCallBackFunc(CCObject* target, SEL_CallFuncN callback) {
	m_callbackListener = target;
	m_Cancelcallback = callback;
}

void MyDialog::chooseMode(int mode) {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto userInput = TextField::create("username", "fonts/Marker Felt.ttf", 40);
	auto passInput = TextField::create("password", "fonts/Marker Felt.ttf", 40);
	auto emailInput = TextField::create("email", "fonts/Marker Felt.ttf", 40);
	// Mode: 0 for signin, 1 for signup, 2 for .....
	switch (mode) {
		case 0:
			labelArr.pushBack(userInput);
			labelArr.pushBack(passInput);
			break;
		case 1:
			labelArr.pushBack(userInput);
			labelArr.pushBack(passInput);
			labelArr.pushBack(emailInput);
			break;
		case 2:
			setSpriteBackGround(Sprite::create("smallBoard.png"));
			break;
		case 3:
			setSpriteBackGround(Sprite::create("smallBoard.png"));
			getMenuButton()->getChildByName("Enter")->removeFromParentAndCleanup(true);
			getMenuButton()->getChildByName("Cancel")->setPosition(Vec2(visibleSize / 2));
			break;
		case 4:
			setSpriteBackGround(Sprite::create("smallBoard.png"));
			getMenuButton()->getChildByName("Cancel")->removeFromParentAndCleanup(true);
			getMenuButton()->getChildByName("Enter")->setPosition(Vec2(visibleSize / 2));
			break;
	}
}

void MyDialog::onEnter() {
	Layer::onEnter();

	Size winSize = Director::getInstance()->getVisibleSize();
	getSpriteBackGround()->setPosition(winSize.width / 2, winSize.height / 2);
	this->addChild(getSpriteBackGround(), 0, 0);
	auto contentSize = getSpriteBackGround()->getTexture()->getContentSize();

	this->addChild(getMenuButton());

	float btnWidth = contentSize.width / (getMenuButton()->getChildrenCount() + 1);
	Vector<Node*> vArray = getMenuButton()->getChildren();
	int i = 0;
	for (auto& e : vArray) {
		Node* node = dynamic_cast<Node*>(e);
		node->setPosition(Point(winSize.width/2 - contentSize.width/2+btnWidth*(i+1), winSize.height-contentSize.height));
		i++;
	}
	int order_height = 300;
	vector<string>::size_type j = 0;
	vector<string> arr;
	arr.push_back("username");
	arr.push_back("password");
	arr.push_back("email");
	if (labelArr.size() > 2) {
		order_height = 380;
	}
	for (auto& a : labelArr) {
		Node* node = dynamic_cast<Node*>(a);
		node->setPosition(Point(winSize.width / 2 , winSize.height - contentSize.height + order_height));
		node->setName(arr[j]);
		this->addChild(node, 1);
		order_height -= 100;
		j++;
	}
	if (labelArr.size() == 0) {
		order_height = 150;
	}
	auto showRes = Label::createWithTTF("", "fonts/Marker Felt.ttf", 35);
	showRes->setPosition(Point(winSize.width / 2, winSize.height - contentSize.height + order_height));
	showRes->setColor(Color3B::RED);
	showRes->setName("Message");
	this->addChild(showRes, 1);
	
	if (getLabelTitle()) {
		getLabelTitle()->setPosition(winSize.width / 2, winSize.height / 2 + (contentSize.height / 2 - 35.0f));
		this->addChild(getLabelTitle());
	}
	Action* popLayer = Sequence::create(ScaleTo::create(0.0, 0.0),
										ScaleTo::create(0.06, 1.05),
										ScaleTo::create(0.08, 0.95),
										ScaleTo::create(0.08, 1.0), NULL);
	this->runAction(popLayer);
}

void MyDialog::changeMsg(std::string mes, bool flag) {
	auto target = (Label*)this->getChildByName("Message");
	target->setString(mes);
	if (flag)
		target->setColor(Color3B::GREEN);
}

void MyDialog::EnbtnCallBack(CCObject* pSender) {
	Node* node = dynamic_cast<Node*>(pSender);
	if (m_callbackListener && m_Entercallback) {
		if (labelArr.size() >= 2) {
			auto username = ((TextField*)this->getChildByName("username"))->getString();
			auto password = ((TextField*)this->getChildByName("password"))->getString();
			if (labelArr.size() > 2) {
				auto email = ((TextField*)this->getChildByName("email"))->getString();
				if (email == "") {
					changeMsg("Please complete all the input fields!", 0);
					return;
				}
				UserDefault::getInstance()->setStringForKey("email", email);
			}
			if (username == "" || password == "") {
				changeMsg("Please complete all the input fields!", 0);
				return;
			}
			else if (username == "username") {
				changeMsg("Do not write name as \'username\'", 0);
				return;
			}
			UserDefault::getInstance()->setStringForKey("username", username);
			UserDefault::getInstance()->setStringForKey("password", password);
		}
		(m_callbackListener->*m_Entercallback)(node);
	}
}

void MyDialog::CabtnCallBack(CCObject* pSender) {
	Node* node = dynamic_cast<Node*>(pSender);
	this->removeFromParentAndCleanup(true);
}

