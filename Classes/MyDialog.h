#pragma once
#ifndef __MY__DIALOG__H__
#define __MY__DIALOG__H__
#include "cocos2d.h"
#include <iostream>
#include "ui\CocosGUI.h"

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace std;

// Self-Design Dialog
class MyDialog : public Layer {
public:
	MyDialog();
	~MyDialog();
	virtual bool init();
	bool onTouchBegan(Touch* t, Event* e);

	// Member Functions
	static MyDialog* create(const char* bgImage);
	void setTitle(const char* title, int fontSize = 30);
	void setEnterCallBackFunc(CCObject* target, SEL_CallFuncN callback);
	void setCancelCallBackFunc(CCObject* target, SEL_CallFuncN callback);
	void chooseMode(int mode);
	void changeMsg(std::string mess, bool flag);

	// Layer Functions
	virtual void onEnter();
	void EnbtnCallBack(CCObject* pSender);
	void CabtnCallBack(CCObject* pSender);

	CREATE_FUNC(MyDialog);

private:
	CCObject* m_callbackListener;
	SEL_CallFuncN m_Entercallback;
	SEL_CallFuncN m_Cancelcallback;
	Vector<TextField*> labelArr;

	CC_SYNTHESIZE_RETAIN(Menu*, m_menu, MenuButton);
	CC_SYNTHESIZE_RETAIN(Sprite*, m_background, SpriteBackGround);
	CC_SYNTHESIZE_RETAIN(LabelTTF*, m_title, LabelTitle);
	
};

#endif