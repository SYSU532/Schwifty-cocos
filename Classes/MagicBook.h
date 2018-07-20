#ifndef MAGIC__BOOK__H__
#define MAGIC__BOOK__H__

#include "cocos2d.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "NetWorkAccess.h"
#include "HelloWorldScene.h"
#include "CardScene.h"
#include "SimpleAudioEngine.h"
#include <vector>

using namespace std;
using namespace cocos2d;
using namespace CocosDenshion;

class MagicBook : public Scene {
public:
	static Scene* createScene();

	virtual bool init();
	
	void backBtn(Ref* rf);
	
	// Card managing
	void initJSONDetails();
	Card* getCardByID(int id);

	// NetWork Listener
	void networkUpdate(float f);

	CREATE_FUNC(MagicBook);

private:
	string myName;
	int rickType;

	vector<Card*> jsonDetails;

};


#endif