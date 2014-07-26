#ifndef __START_SCENE_H__
#define __START_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

class Start : public Scene
{
public:
	Start();
	~Start();
	CREATE_FUNC(Start);

	virtual bool init() override;
	
};

#endif