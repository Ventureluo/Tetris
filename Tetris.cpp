#include "Tetris.h"
#include <time.h>
#include <stdlib.h>
#include <graphics.h>
#include "Block.h"
#include <iostream>
#include <conio.h>
#include <fstream>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
using namespace std;

#define MAX_LEVEL 5
#define RECORDER_FILE "recorder.txt"


const int SPEED_NORMAL[MAX_LEVEL] = {500,300,150,100,80};
const int SPEED_QUICK = 50;//ms

Tetris::Tetris(int rows, int cols, int left, int top, int blockSize)
{
	//
	this->rows = rows;
	this->cols = cols;
	this->leftMargin = left;
	this->topMargin = top;
	this->blockSize = blockSize;
	//初始化地图，若值为0，则无方块
	for (int i = 0; i < rows; i++) {
		vector<int> mapRow;
		for (int j = 0; j < cols; j++) {
			mapRow.push_back(0);
		}
		map.push_back(mapRow);
	}
}

void Tetris::init()
{
	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	delay = SPEED_NORMAL[0];
	//配置随机种子
	srand(time(NULL));

	//创建游戏窗口
	initgraph(938,896);

	//加载背景图片
	loadimage(&imgBg, "res/bg2.png");
	loadimage(&imgWin, "res/win.png");
	loadimage(&imgOver, "res/over.png");

	//初始化游戏区数据
	char data[20][10];
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			map[i][j] = 0;
		}
	}

	score = 0;
	lineCount = 0;
	level = 1;

	//读取文件获得最高分记录
	ifstream file("RECORDER_FILE");
	if (!file.is_open()) {
		cout << RECORDER_FILE << "打开失败" << endl;
		highestScore = 0;
	}
	else {
		file >> highestScore;
	}
	file.close();

}

void Tetris::play()
{
	init();


	nextBlock = new Block;
	curBlock = nextBlock;
	nextBlock = new Block;

	int timer = 0;
	while (1) {
		//接收用户输入
		keyEvent();
	
		timer += getDelay();
		if (timer > delay) {
			timer = 0;
			drop();
			//渲染游戏画面
			update = true;
		}

		if (update) {
			update = false;
			//更新游戏画面
			updateWindow();
			//更新游戏数据
			clearLine();
		}
		if (gameOver) {
			//保存分数
			saveScore();

			//显示游戏结束界面
			displayOver();

			system("pause");
			gameOver = true;
			init();//重开
		}
		
	}
}

void Tetris::keyEvent()
{
	unsigned char ch;//普通的char只能-127,128
	bool rotateFlag = false;
	int dx = 0;
	if (_kbhit()) {
		ch=_getch();
		//按下方向键，会返回两个字符
		//上方向键，依次返回：224 72
		//下方向键，依次返回：224 80
		//左方向键，依次返回：224 75
		//右方向键，依次返回：224 77
		if (ch == 224) {
			ch = _getch();
			switch (ch)
			{
			case 72: 
				rotateFlag = true; break;
			case 80:
				delay = SPEED_QUICK; break;
			case 75:
				dx = -1;break;
			case 77:
				dx = 1;break;
			default:break;
			}
		}
	}
	if (rotateFlag) {
		//旋转
		rotate();
		update = true;
	}

	if (dx != 0) {
		//左右移动
		moveLeftRight(dx);
		update = true;
	}
}

void Tetris::updateWindow()
{
	putimage(0, 0, &imgBg);//绘制背景图片

	//测试方块
	//Block block;
	//block.draw(leftMargin,topMargin);
	IMAGE** imgs = Block::getImages();
	BeginBatchDraw();//避免画面闪烁，计算完再一次性渲染
	for (int i = 0; i < rows; i++){
		for (int j = 0; j < cols; j++) {
			if (map[i][j] == 0)continue;

			int x = j * blockSize + leftMargin;
			int y = i * blockSize + topMargin;
			putimage(x, y, imgs[map[i][j]-1]);//减一

		}
	}


	curBlock->draw(leftMargin, topMargin);
	nextBlock->draw(689,150);

	drawScore();

	EndBatchDraw();
}

//第一次调用返回0，
//返回距离上一次调用，间隔了多少ms
int Tetris::getDelay()
{
	static unsigned long long lastTime = 0;
	unsigned long long currentTime=GetTickCount();
	
	if (lastTime == 0) {
		lastTime = currentTime;
		return 0;
	}
	else {
		int ret = currentTime - lastTime;
		lastTime = currentTime;
		return ret;
	}
}

void Tetris::drop()
{
	bakBlock = *curBlock;
	curBlock->drop();

	//下降过程中,违法位置
	if (!curBlock->blockInMap(map)) {
		//固化方块
		bakBlock.solidify(map);
		delay = SPEED_NORMAL[level-1];//加速固化之后恢复速度
		delete curBlock;
		curBlock = nextBlock;
		nextBlock = new Block;

		//检查游戏结束
		checkOver();
	}
	//如果delay = SPEED_NORMAL放在此处实现按住下键加速，松开还原
}

void Tetris::clearLine()
{
	int lines = 0;
	int k = rows - 1;//存储数据的行数
	for (int i = rows - 1; i >= 0; i--) {
		//检查第i行是否满行
		int count = 0;
		for (int j = 0; j < cols; j++) {
			if (map[i][j]) {
				count++;
			}
			map[k][j] = map[i][j];
			//一边扫描一边存储,满行的话k不变，上层覆盖到原k层

		}

		if (count < cols) {//不满行
			k--;
		}
		else {//count==cols,满行
			lines++;
		}
	}
	lineCount += lines;

	if (lines > 0) {
		//得分
		int addScore[4] = { 10,30,60,80 };
		score += addScore[lines - 1];

		mciSendString("play res/xiaochu1.mp3", 0, 0, 0);
		update = true;

		//每一百分为一关
		level = score / 100 + 1;
		if (level > MAX_LEVEL) {
			gameOver = true;
		}
	}
}

void Tetris::moveLeftRight(int offset)
{
	bakBlock = *curBlock;
	curBlock->moveLeftRight(offset);
	if (!curBlock->blockInMap(map)) {
		*curBlock = bakBlock;
	}
}

void Tetris::rotate()
{
	if (curBlock->getBlockType() == 7) return;//田字形没必要转

	bakBlock = *curBlock;
	curBlock->rotate();
	if (!curBlock->blockInMap(map)) {
		*curBlock = bakBlock;
	}
}

void Tetris::drawScore()
{
	char scoreText[32];
	sprintf_s(scoreText, sizeof(scoreText), "%d", score);//格式化输出为字符串

	setcolor(RGB(180, 180, 180));
	LOGFONT f;
	gettextstyle(&f);//获取当前字体
	f.lfHeight = 60;
	f.lfWidth = 30;
	f.lfQuality = ANTIALIASED_QUALITY;//设置字体为“抗锯齿”，
	strcpy_s(f.lfFaceName, sizeof(f.lfFaceName), _T("Segoe UI Black"));
	settextstyle(&f);

	setbkmode(TRANSPARENT);//设置字体透明

	outtextxy(670,727,scoreText);//绘制分数

	sprintf_s(scoreText, sizeof(scoreText), "%d", lineCount);
	gettextstyle(&f);//消除的行数显示右对其
	int xPos = 224 - f.lfWidth * strlen(scoreText);
	outtextxy(xPos, 817, scoreText);//绘制消除行数

	sprintf_s(scoreText, sizeof(scoreText), "%d", level);
	outtextxy(194, 727, scoreText);//绘制第几关

	sprintf_s(scoreText, sizeof(scoreText), "%d", highestScore);
	outtextxy(670, 817, scoreText);//绘制最高分
}

void Tetris::checkOver()
{
	gameOver = (curBlock->blockInMap(map) == false);
}

void Tetris::saveScore()
{
	if (score > highestScore) {
		highestScore = score;

		ofstream file(RECORDER_FILE);
		file << highestScore;
		file.close();
	}
}

void Tetris::displayOver()
{
	mciSendString("stop res/bg.mp3", 0, 0, 0);

	//胜利结束还是失败结束
	if (level <= MAX_LEVEL) {
		putimage(262, 361, &imgOver);
		mciSendString("play res/over.mp3", 0, 0, 0);
	}
	else {
		putimage(262, 361, &imgWin);
		mciSendString("play res/win.mp3", 0, 0, 0);
	}
}
