#pragma once
#include <vector>
#include <graphics.h>
#include "Block.h"
using namespace std;
class Tetris
{
public:
	//构造函数,游戏行列，边框，方块大小
	Tetris(int rows,int cols,int left,int top,int blockSize);
	void init();//初始化
	void play();//开始游戏

private:
	void keyEvent();
	void updateWindow();
	//返回上一次调用该函数间隔了多少时间（ms）
	//第一次调用返回0
	int getDelay();
	void drop();
	void clearLine();
	void moveLeftRight(int offset);//左右移动
	void rotate();//旋转
	void drawScore();//绘制当前分数
	void checkOver();//检查游戏是否结束
	void saveScore();//保存最高分
	void displayOver();//更新游戏结束画面

private:
	int delay;
	bool update;//是否更新

	//背景图
	vector<vector<int>> map;
	int rows;
	int cols;
	int leftMargin;
	int topMargin;
	int blockSize;
	IMAGE imgBg;

	//当前方块和预告方块
	Block* curBlock;
	Block* nextBlock;
	Block bakBlock;//当前方块降落过程中，用来备份上一个合法位置的

	int score;//当前分数
	int highestScore;//最高
	int level;//当前关卡
	int lineCount;//消除行数
	bool gameOver;//游戏是否结束

	IMAGE imgOver;
	IMAGE imgWin;
};

