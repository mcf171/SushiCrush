#include "PlayLayer.h"
#include "StartScene.h"
#include "SushiSprite.h"

#define MATRIX_WIDTH (7)
#define MATRIX_HEIGHT (9)

#define SUSHI_GAP (1)

PlayLayer::PlayLayer()
: spriteSheet(NULL)
, m_matrix(NULL)
, m_width(0)
, m_height(0)
, mScore(0)
, m_matrixLeftBottomX(0)
, m_matrixLeftBottomY(0)
, m_isNeedFillVacancies(false)
, m_isNeedRemove(false)
, m_isAnimationing(true)//start with drop animation
, m_isTouchEnable(true)
, m_srcSushi(NULL)
, m_destSushi(NULL)
, m_movingVertical(true)//drop animation is vertical
{
}

PlayLayer::~PlayLayer()
{
	if (m_matrix) {
		free(m_matrix);
	}
}

/*
Scene *PlayLayer::createScene()
{
	auto scene = Scene::create();
	auto layer = PlayLayer::create();
	scene->addChild(layer);
	return scene;
}
*/

bool PlayLayer::init()
{
	if (!Layer::init()) {
		return false;
	}

	// background
	Size winSize = Director::getInstance()->getWinSize();
	auto background = Sprite::create("background.png");
	background->setAnchorPoint(Point(0, 1));
	background->setPosition(Point(0, winSize.height));
	playlayer->addChild(background, -1);

	// init spriteSheet
	srand(time(NULL));
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sushi.plist");
	spriteSheet = SpriteBatchNode::create("sushi.pvr.ccz");
	addChild(spriteSheet);

	// init score
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	__String* ns = __String::createWithFormat("%d",mScore);
	std::string s = ns->getCString();
	Scorelab = LabelAtlas::create(s,"num.png",7,10,'0');
	Scorelab->setPosition(Vec2(origin.x + Scorelab->getContentSize().width/2,
							origin.y + visibleSize.height - Scorelab->getContentSize().height));
	this->addChild(Scorelab,1);

	// init menu
	auto closeItem = MenuItemImage::create(
	                                       "CloseNormal.png",
	                                       "CloseSelected.png",
	                                       CC_CALLBACK_1(PlayLayer::menuCloseCallback, this));

	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
	                          	  origin.y + closeItem->getContentSize().height/2));

	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	// Yes, you can load this value from other module.
	m_width = MATRIX_WIDTH;
	m_height = MATRIX_HEIGHT;

	// init position value
	m_matrixLeftBottomX = (winSize.width - SushiSprite::getContentWidth() * m_width - (m_width - 1) * SUSHI_GAP) / 2;
	m_matrixLeftBottomY = (winSize.height - SushiSprite::getContentWidth() * m_height - (m_height - 1) * SUSHI_GAP) / 2;

	// init point array
	int arraySize = sizeof(SushiSprite *)* m_width * m_height;
	m_matrix = (SushiSprite **)malloc(arraySize);
	memset((void*)m_matrix, 0, arraySize);

	initMatrix();
	scheduleUpdate();

	// bind touch event
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(PlayLayer::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(PlayLayer::onTouchMoved, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	return true;
}

SushiSprite *PlayLayer::sushiOfPoint(Point *point)
{
	SushiSprite *sushi = NULL;
	Rect rect = Rect(0, 0, 0, 0);

	for (int i = 0; i < m_height * m_width; i++) {
		sushi = m_matrix[i];
		if (sushi) {
			rect.origin.x = sushi->getPositionX() - (sushi->getContentSize().width / 2);
			rect.origin.y = sushi->getPositionY() - (sushi->getContentSize().height / 2);
			rect.size = sushi->getContentSize();
			if (rect.containsPoint(*point)) {
				return sushi;
			}
		}
	}

	return NULL;
}

bool PlayLayer::onTouchBegan(Touch *touch, Event *unused)
{
	m_srcSushi = NULL;
	m_destSushi = NULL;
	if (m_isTouchEnable) {
		auto location = touch->getLocation();
		m_srcSushi = sushiOfPoint(&location);
	}
	return m_isTouchEnable;
}

void PlayLayer::onTouchMoved(Touch *touch, Event *unused)
{
	if (!m_srcSushi || !m_isTouchEnable) {
		return;
	}

	int row = m_srcSushi->getRow();
	int col = m_srcSushi->getCol();

	auto location = touch->getLocation();
	auto halfSushiWidth = m_srcSushi->getContentSize().width / 2;
	auto halfSushiHeight = m_srcSushi->getContentSize().height / 2;

	auto  upRect = Rect(m_srcSushi->getPositionX() - halfSushiWidth,
		m_srcSushi->getPositionY() + halfSushiHeight,
		m_srcSushi->getContentSize().width,
		m_srcSushi->getContentSize().height);

	if (upRect.containsPoint(location)) {
		row++;
		if (row < m_height) {
			m_destSushi = m_matrix[row * m_width + col];
		}
		m_movingVertical = true;
		swapSushi();
		return;
	}

	auto  downRect = Rect(m_srcSushi->getPositionX() - halfSushiWidth,
		m_srcSushi->getPositionY() - (halfSushiHeight * 3),
		m_srcSushi->getContentSize().width,
		m_srcSushi->getContentSize().height);

	if (downRect.containsPoint(location)) {
		row--;
		if (row >= 0) {
			m_destSushi = m_matrix[row * m_width + col];
		}
		m_movingVertical = true;
		swapSushi();
		return;
	}

	auto  leftRect = Rect(m_srcSushi->getPositionX() - (halfSushiWidth * 3),
		m_srcSushi->getPositionY() - halfSushiHeight,
		m_srcSushi->getContentSize().width,
		m_srcSushi->getContentSize().height);

	if (leftRect.containsPoint(location)) {
		col--;
		if (col >= 0) {
			m_destSushi = m_matrix[row * m_width + col];
		}
		m_movingVertical = false;
		swapSushi();
		return;
	}

	auto  rightRect = Rect(m_srcSushi->getPositionX() + halfSushiWidth,
		m_srcSushi->getPositionY() - halfSushiHeight,
		m_srcSushi->getContentSize().width,
		m_srcSushi->getContentSize().height);

	if (rightRect.containsPoint(location)) {
		col++;
		if (col < m_width) {
			m_destSushi = m_matrix[row * m_width + col];
		}
		m_movingVertical = false;
		swapSushi();
		return;
	}

	// not a useful movement
}

void PlayLayer::swapSushi()
{
	m_isAnimationing = true;
	m_isTouchEnable = false;
	if (!m_srcSushi || !m_destSushi) {
		m_movingVertical = true;
		return;
	}

	Point posOfSrc = m_srcSushi->getPosition();
	Point posOfDest = m_destSushi->getPosition();
	float time = 0.2;

	// 1.swap in matrix
	m_matrix[m_srcSushi->getRow() * m_width + m_srcSushi->getCol()] = m_destSushi;
	m_matrix[m_destSushi->getRow() * m_width + m_destSushi->getCol()] = m_srcSushi;
	int tmpRow = m_srcSushi->getRow();
	int tmpCol = m_srcSushi->getCol();
	m_srcSushi->setRow(m_destSushi->getRow());
	m_srcSushi->setCol(m_destSushi->getCol());
	m_destSushi->setRow(tmpRow);
	m_destSushi->setCol(tmpCol);

	// 2.check for remove able
	std::list<SushiSprite *> colChainListOfFirst;
	getColChain(m_srcSushi, colChainListOfFirst);

	std::list<SushiSprite *> rowChainListOfFirst;
	getRowChain(m_srcSushi, rowChainListOfFirst);

	std::list<SushiSprite *> colChainListOfSecond;
	getColChain(m_destSushi, colChainListOfSecond);

	std::list<SushiSprite *> rowChainListOfSecond;
	getRowChain(m_destSushi, rowChainListOfSecond);

	if (colChainListOfFirst.size() >= 3
		|| rowChainListOfFirst.size() >= 3
		|| colChainListOfSecond.size() >= 3
		|| rowChainListOfSecond.size() >= 3) {
		// just swap
		m_srcSushi->runAction(MoveTo::create(time, posOfDest));
		m_destSushi->runAction(MoveTo::create(time, posOfSrc));
		return;
	}

	// 3.no chain, swap back
	m_matrix[m_srcSushi->getRow() * m_width + m_srcSushi->getCol()] = m_destSushi;
	m_matrix[m_destSushi->getRow() * m_width + m_destSushi->getCol()] = m_srcSushi;
	tmpRow = m_srcSushi->getRow();
	tmpCol = m_srcSushi->getCol();
	m_srcSushi->setRow(m_destSushi->getRow());
	m_srcSushi->setCol(m_destSushi->getCol());
	m_destSushi->setRow(tmpRow);
	m_destSushi->setCol(tmpCol);

	m_srcSushi->runAction(Sequence::create(
		MoveTo::create(time, posOfDest),
		MoveTo::create(time, posOfSrc),
		NULL));
	m_destSushi->runAction(Sequence::create(
		MoveTo::create(time, posOfSrc),
		MoveTo::create(time, posOfDest),
		NULL));
}

void PlayLayer::update(float dt)
{
	// check if animationing
	if (m_isAnimationing) {
		// update Score
		__String* ns = __String::createWithFormat("%d", mScore);
		std::string s = ns->getCString();
		Scorelab->setString(s);
		// init with false
		m_isAnimationing = false;
		for (int i = 0; i < m_height * m_width; i++) {
			SushiSprite *sushi = m_matrix[i];
			if (sushi && sushi->getNumberOfRunningActions() > 0) {
				m_isAnimationing = true;
				break;
			}
		}
	}


	if (!m_isAnimationing) {
		if (m_isNeedFillVacancies) {
			//爆炸效果结束后才掉落新寿司，增强打击感
			fillVacancies();
			m_isNeedFillVacancies = false;
		}
		else {
			checkAndRemoveChain();
			// if sushi is moving or befor fill and check, ignore use touch event
			m_isTouchEnable = !m_isAnimationing;
		}
	}
}

//横向chain检查
void PlayLayer::getColChain(SushiSprite *sushi, std::list<SushiSprite *> &chainList)
{
	chainList.push_back(sushi);// add first sushi

	int neighborCol = sushi->getCol() - 1;
	int index;
	if (sushi->getDisplayMode() == DISPLAY_MODE_FIVECRUSH);
		

	while (neighborCol >= 0) {
		SushiSprite *neighborSushi = m_matrix[sushi->getRow() * m_width + neighborCol];
		if (neighborSushi
			&& (neighborSushi->getImgIndex() == sushi->getImgIndex())
			|| neighborSushi->getDisplayMode() == DISPLAY_MODE_FIVECRUSH) {
			chainList.push_back(neighborSushi);
			neighborCol--;
		}
		else {
			break;
		}
	}

	neighborCol = sushi->getCol() + 1;
	while (neighborCol < m_width) {
		SushiSprite *neighborSushi = m_matrix[sushi->getRow() * m_width + neighborCol];
		if (neighborSushi
			&& (neighborSushi->getImgIndex() == sushi->getImgIndex())
			|| neighborSushi->getDisplayMode() == DISPLAY_MODE_FIVECRUSH) {
			chainList.push_back(neighborSushi);
			neighborCol++;
		}
		else {
			break;
		}
	}
}

void PlayLayer::getRowChain(SushiSprite *sushi, std::list<SushiSprite *> &chainList)
{
	chainList.push_back(sushi);// add first sushi

	int neighborRow = sushi->getRow() - 1;
	while (neighborRow >= 0) {
		SushiSprite *neighborSushi = m_matrix[neighborRow * m_width + sushi->getCol()];
		if (neighborSushi
			&& (neighborSushi->getImgIndex() == sushi->getImgIndex())
			|| neighborSushi->getDisplayMode() == DISPLAY_MODE_FIVECRUSH) {
			chainList.push_back(neighborSushi);
			neighborRow--;
		}
		else {
			break;
		}
	}

	neighborRow = sushi->getRow() + 1;
	while (neighborRow < m_height) {
		SushiSprite *neighborSushi = m_matrix[neighborRow * m_width + sushi->getCol()];
		if (neighborSushi
			&& (neighborSushi->getImgIndex() == sushi->getImgIndex())
			|| neighborSushi->getDisplayMode() == DISPLAY_MODE_FIVECRUSH) {
			chainList.push_back(neighborSushi);
			neighborRow++;
		}
		else {
			break;
		}
	}
}

void PlayLayer::fillVacancies()
{
	// reset moving direction flag
	m_movingVertical = true;
	m_isAnimationing = true;

	Size size = CCDirector::getInstance()->getWinSize();
	int *colEmptyInfo = (int *)malloc(sizeof(int)* m_width);
	memset((void *)colEmptyInfo, 0, sizeof(int)* m_width);

	// 1. drop exist sushi
	SushiSprite *sushi = NULL;
	for (int col = 0; col < m_width; col++) {
		int removedSushiOfCol = 0;
		// from buttom to top
		for (int row = 0; row < m_height; row++) {
			sushi = m_matrix[row * m_width + col];
			if (NULL == sushi) {
				removedSushiOfCol++;
			}
			else {
				if (removedSushiOfCol > 0) {
					// evey item have its own drop distance
					int newRow = row - removedSushiOfCol;
					// switch in matrix
					m_matrix[newRow * m_width + col] = sushi;
					m_matrix[row * m_width + col] = NULL;
					// move to new position
					Vec2 endPosition = positionOfItem(newRow, col);
					Vec2 startPosition = sushi->getPosition();
					float speed = (startPosition.y - endPosition.y) / size.height;
					sushi->stopAllActions();// must stop pre drop action
					sushi->runAction(CCMoveTo::create(speed, endPosition));
					// set the new row to item
					sushi->setRow(newRow);
				}
			}
		}

		// record empty info
		colEmptyInfo[col] = removedSushiOfCol;
	}

	// 2. create new item and drop down.
	for (int col = 0; col < m_width; col++) {
		for (int row = m_height - colEmptyInfo[col]; row < m_height; row++) {
			createAndDropSushi(row, col);
		}
	}

	free(colEmptyInfo);
}

void PlayLayer::removeSushi()
{
	// make sequence remove
	m_isAnimationing = true;

	for (int i = 0; i < m_height * m_width; i++) {
		SushiSprite *sushi = m_matrix[i];
		if (!sushi) {
			continue;
		}

		if (sushi->getIsNeedRemove()) {
			m_isNeedFillVacancies = true;//需要掉落
			// TODO: 检查类型，并播放一行消失的动画

			if (sushi->getDisplayMode() == DISPLAY_MODE_HORIZONTAL || sushi->getDisplayMode() == DISPLAY_MODE_FIVECRUSH)
			{
				explodeSpecialH(sushi->getPosition());
			}
			if (sushi->getDisplayMode() == DISPLAY_MODE_VERTICAL || sushi->getDisplayMode() == DISPLAY_MODE_FIVECRUSH)
			{
				explodeSpecialV(sushi->getPosition());
			}
			explodeSushi(sushi);
			mScore++;

		}
	}
}

void PlayLayer::explodeSpecialH(Point point)
{
	Size size = Director::getInstance()->getWinSize();
	float scaleX = 4;
	float scaleY = 0.7;
	float time = 0.3;
	Point startPosition = point;
	float speed = 0.6f;

	auto colorSpriteRight = Sprite::create("colorHRight.png");
	addChild(colorSpriteRight, 10);
	Point endPosition1 = Point(point.x - size.width, point.y);
	colorSpriteRight->setPosition(startPosition);
	colorSpriteRight->runAction(Sequence::create(ScaleTo::create(time, scaleX, scaleY),
		MoveTo::create(speed, endPosition1),
		CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, colorSpriteRight)),
		NULL));

	auto colorSpriteLeft = Sprite::create("colorHLeft.png");
	addChild(colorSpriteLeft, 10);
	Point endPosition2 = Point(point.x + size.width, point.y);
	colorSpriteLeft->setPosition(startPosition);
	colorSpriteLeft->runAction(Sequence::create(ScaleTo::create(time, scaleX, scaleY),
		MoveTo::create(speed, endPosition2),
		CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, colorSpriteLeft)),
		NULL));

	mScore += 20;
}

void PlayLayer::explodeSpecialV(Point point)
{
	Size size = Director::getInstance()->getWinSize();
	float scaleY = 4;
	float scaleX = 0.7;
	float time = 0.3;
	Point startPosition = point;
	float speed = 0.6f;

	auto colorSpriteDown = Sprite::create("colorVDown.png");
	addChild(colorSpriteDown, 10);
	Point endPosition1 = Point(point.x, point.y - size.height);
	colorSpriteDown->setPosition(startPosition);
	colorSpriteDown->runAction(Sequence::create(ScaleTo::create(time, scaleX, scaleY),
		MoveTo::create(speed, endPosition1),
		CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, colorSpriteDown)),
		NULL));

	auto colorSpriteUp = Sprite::create("colorVUp.png");
	addChild(colorSpriteUp, 10);
	Point endPosition2 = Point(point.x, point.y + size.height);
	colorSpriteUp->setPosition(startPosition);
	colorSpriteUp->runAction(Sequence::create(ScaleTo::create(time, scaleX, scaleY),
		MoveTo::create(speed, endPosition2),
		CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, colorSpriteUp)),
		NULL));

	mScore += 20;
}

void PlayLayer::actionEndCallback(Node *node)
{
	SushiSprite *sushi = (SushiSprite *)node;
	m_matrix[sushi->getRow() * m_width + sushi->getCol()] = NULL;
	sushi->removeFromParent();
}

void PlayLayer::explodeSushi(SushiSprite *sushi)
{
	float time = 0.3;

	// 1. action for sushi
	sushi->runAction(Sequence::create(
		ScaleTo::create(time, 0.0),
		CallFuncN::create(CC_CALLBACK_1(PlayLayer::actionEndCallback, this)),
		NULL));

	// 2. action for circle
	auto circleSprite = Sprite::create("circle.png");
	addChild(circleSprite, 10);
	circleSprite->setPosition(sushi->getPosition());
	circleSprite->setScale(0);// start size
	circleSprite->runAction(Sequence::create(ScaleTo::create(time, 1.0),
		CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, circleSprite)),
		NULL));

	// 3. particle effect
	auto particleStars = ParticleSystemQuad::create("stars.plist");
	particleStars->setAutoRemoveOnFinish(true);
	particleStars->setBlendAdditive(false);
	particleStars->setPosition(sushi->getPosition());
	particleStars->setScale(0.3);
	addChild(particleStars, 20);
}

void PlayLayer::checkAndRemoveChain()
{
	SushiSprite *sushi;
	// 1. reset ingnore flag and m_isNeedRemove
	for (int i = 0; i < m_height * m_width; i++) {
		sushi = m_matrix[i];
		if (!sushi) {
			continue;
		}
		sushi->setIgnoreCheck(false);
	}
	m_isNeedRemove = false;
	
	// 2. check chain
	for (int i = 0; i < m_height * m_width; i++) {
		sushi = m_matrix[i];
		if (!sushi) {
			continue;
		}

		if (sushi->getIsNeedRemove()) {
			continue;// 已标记过的跳过检查
		}
		if (sushi->getIgnoreCheck()) {
			continue;// 新变化的特殊寿司，不消除
		}

		// start count chain
		std::list<SushiSprite *> colChainList;
		getColChain(sushi, colChainList);

		std::list<SushiSprite *> rowChainList;
		getRowChain(sushi, rowChainList);

		std::list<SushiSprite *> &longerList = colChainList.size() > rowChainList.size() ? colChainList : rowChainList;
		if (longerList.size() < 3) {
			continue;// 小于3个不消除
		}

		std::list<SushiSprite *>::iterator itList;
		bool isSetedIgnoreCheck = false;
		for (itList = longerList.begin(); itList != longerList.end(); itList++) {
			sushi = (SushiSprite *)*itList;
			if (!sushi) {
				continue;
			}

			if (longerList.size() > 4) {
				// 5消产生特殊寿司
				if (sushi == m_srcSushi || sushi == m_destSushi) {
					isSetedIgnoreCheck = true;
					sushi->setIgnoreCheck(true);
					sushi->setIsNeedRemove(false);
					sushi->setDisplayMode(DISPLAY_MODE_FIVECRUSH);
					continue;
				}
			}

			if (longerList.size() == 4) {
				// 4消产生特殊寿司
				if (sushi == m_srcSushi || sushi == m_destSushi) {
					isSetedIgnoreCheck = true;
					sushi->setIgnoreCheck(true);
					sushi->setIsNeedRemove(false);
					sushi->setDisplayMode(m_movingVertical ? DISPLAY_MODE_VERTICAL : DISPLAY_MODE_HORIZONTAL);
					continue;
				}
			}

			m_isNeedRemove = true;
			markRemove(sushi);
		}

		// 取最后一个变化的为自由掉落产生的特殊寿司
		if (!isSetedIgnoreCheck && longerList.size() > 3) {
			sushi->setIgnoreCheck(true);
			sushi->setIsNeedRemove(false);
			if (longerList.size() == 4)
				sushi->setDisplayMode(m_movingVertical ? DISPLAY_MODE_VERTICAL : DISPLAY_MODE_HORIZONTAL);
			else
				sushi->setDisplayMode(DISPLAY_MODE_FIVECRUSH);
		}
	}

	// 3.消除标记了的寿司
	if (m_isNeedRemove)
		removeSushi();

}

void PlayLayer::markRemove(SushiSprite *sushi)
{
	if (sushi->getIsNeedRemove()) {
		return;
	}
	if (sushi->getIgnoreCheck()) {
		return;
	}

	// mark self
	sushi->setIsNeedRemove(true);
	// check for type and mark for certical neighbour
	if (sushi->getDisplayMode() == DISPLAY_MODE_VERTICAL || sushi->getDisplayMode() == DISPLAY_MODE_FIVECRUSH) {
		for (int row = 0; row < m_height; row++) {
			SushiSprite *tmp = m_matrix[row * m_width + sushi->getCol()];
			if (!tmp || tmp == sushi) {
				continue;
			}

			if (tmp->getDisplayMode() == DISPLAY_MODE_NORMAL) {
				tmp->setIsNeedRemove(true);
			}
			else {
				markRemove(tmp);
			}
		}
		// check for type and mark for horizontal neighbour
	}
	if (sushi->getDisplayMode() == DISPLAY_MODE_HORIZONTAL || sushi->getDisplayMode() == DISPLAY_MODE_FIVECRUSH) {
		for (int col = 0; col < m_width; col++) {
			SushiSprite *tmp = m_matrix[sushi->getRow() * m_width + col];
			if (!tmp || tmp == sushi) {
				continue;
			}

			if (tmp->getDisplayMode() == DISPLAY_MODE_NORMAL) {
				tmp->setIsNeedRemove(true);
			}
			else {
				markRemove(tmp);
			}
		}
	}
}

void PlayLayer::initMatrix()
{
	for (int row = 0; row < m_height; row++) {
		for (int col = 0; col < m_width; col++) {
			createAndDropSushi(row, col);
		}
	}
}

void PlayLayer::createAndDropSushi(int row, int col)
{
	Size size = Director::getInstance()->getWinSize();

	SushiSprite *sushi = SushiSprite::create(row, col);

	// create animation
	Point endPosition = positionOfItem(row, col);
	Point startPosition = Point(endPosition.x, endPosition.y + size.height / 2);
	sushi->setPosition(startPosition);
	float speed = startPosition.y / (1.5 * size.height);
	sushi->runAction(MoveTo::create(speed, endPosition));
	// add to BatchNode
	spriteSheet->addChild(sushi);

	m_matrix[row * m_width + col] = sushi;
}

Point PlayLayer::positionOfItem(int row, int col)
{
	float x = m_matrixLeftBottomX + (SushiSprite::getContentWidth() + SUSHI_GAP) * col + SushiSprite::getContentWidth() / 2;
	float y = m_matrixLeftBottomY + (SushiSprite::getContentWidth() + SUSHI_GAP) * row + SushiSprite::getContentWidth() / 2;
	return Point(x, y);
}

void PlayLayer::menuCloseCallback(Ref* pSender)
{
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
		MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
		return;
	#endif

    Director::getInstance()->end();

	#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    	exit(0);
	#endif
}
