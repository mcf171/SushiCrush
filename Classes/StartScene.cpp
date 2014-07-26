#include "StartScene.h"
#include "PlayLayer.h"
#include "StartLayer.h"

Start::Start()
{
}

Start::~Start()
{
}

bool Start::init()
{
	auto startlayer = StartLayer::create();
	this->addChild(startlayer);

	return true;
}