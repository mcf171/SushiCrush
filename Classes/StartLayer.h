#ifndef __START_LAYER_H__
#define __START_LAYER_H__

#include "cocos2d.h"

USING_NS_CC;

class StartLayer : public Layer
{
public:
	StartLayer();
	~StartLayer();
	CREATE_FUNC(StartLayer);

	virtual bool init() override;
	void newgameCallback(cocos2d::Ref* pSender);
	void continueCallback(cocos2d::Ref* pSender);
	void settingsCallback(cocos2d::Ref* pSender);
	void exitCallback(cocos2d::Ref* pSender);

};

#endif