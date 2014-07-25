#ifndef __PlayLayer_H__
#define __PlayLayer_H__

#include "cocos2d.h"

USING_NS_CC;

class SushiSprite;

class PlayLayer : public Layer
{
public:
	PlayLayer();
	~PlayLayer();
	//static Scene* createScene();
	void menuCloseCallback(Ref* pSender);
	CREATE_FUNC(PlayLayer);

	virtual bool init() override;
	virtual void update(float dt) override;
	virtual bool onTouchBegan(Touch *touch, Event *unused) override;
	virtual void onTouchMoved(Touch *touch, Event *unused) override;
private:
	SpriteBatchNode *spriteSheet;
	SushiSprite **m_matrix;// 2D array which store (SushiSprite *).
	LabelAtlas* Scorelab;
	int m_width;
	int m_height;
	int mScore;
	// for rapid count position
	float m_matrixLeftBottomX;
	float m_matrixLeftBottomY;
	bool m_isTouchEnable;// is need to deal with touch event
	SushiSprite *m_srcSushi;// �û��ƶ�����˾
	SushiSprite *m_destSushi;// �ƶ����ĸ���˾
	bool m_isAnimationing;
	bool m_isNeedFillVacancies;//�Ƿ��п�ȱ��Ҫ�
	bool m_isNeedRemove;//�Ƿ�����˾��Ҫ����
	bool m_movingVertical;// true: 4������������ը��.  false: 4������������ը��.

	void initMatrix();
	void createAndDropSushi(int row, int col);
	Point positionOfItem(int row, int col);
	void checkAndRemoveChain();
	void getColChain(SushiSprite *sushi, std::list<SushiSprite *> &chainList);
	void getRowChain(SushiSprite *sushi, std::list<SushiSprite *> &chainList);
	void removeSushi();
	void actionEndCallback(Node *node);
	void explodeSushi(SushiSprite *sushi);

	void explodeSpecialH(Point point);
	void explodeSpecialV(Point point);

	void fillVacancies();
	SushiSprite *sushiOfPoint(Point *point);
	void swapSushi();
	void markRemove(SushiSprite *sushi);
};

#endif /* defined(__PlayLayer_H__) */
