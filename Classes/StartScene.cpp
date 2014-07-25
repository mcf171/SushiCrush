#include "StartScene.h"
#include "PlayLayer.h"

Start::Start()
{
}

Start::~Start()
{
}

bool Start::init()
{
	auto playlayer = PlayLayer::create();
	this->addChild(playlayer);

	return true;
}