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
	//��ʼ����ͼ����ֵΪ0�����޷���
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
	//�����������
	srand(time(NULL));

	//������Ϸ����
	initgraph(938,896);

	//���ر���ͼƬ
	loadimage(&imgBg, "res/bg2.png");
	loadimage(&imgWin, "res/win.png");
	loadimage(&imgOver, "res/over.png");

	//��ʼ����Ϸ������
	char data[20][10];
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			map[i][j] = 0;
		}
	}

	score = 0;
	lineCount = 0;
	level = 1;

	//��ȡ�ļ������߷ּ�¼
	ifstream file("RECORDER_FILE");
	if (!file.is_open()) {
		cout << RECORDER_FILE << "��ʧ��" << endl;
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
		//�����û�����
		keyEvent();
	
		timer += getDelay();
		if (timer > delay) {
			timer = 0;
			drop();
			//��Ⱦ��Ϸ����
			update = true;
		}

		if (update) {
			update = false;
			//������Ϸ����
			updateWindow();
			//������Ϸ����
			clearLine();
		}
		if (gameOver) {
			//�������
			saveScore();

			//��ʾ��Ϸ��������
			displayOver();

			system("pause");
			gameOver = true;
			init();//�ؿ�
		}
		
	}
}

void Tetris::keyEvent()
{
	unsigned char ch;//��ͨ��charֻ��-127,128
	bool rotateFlag = false;
	int dx = 0;
	if (_kbhit()) {
		ch=_getch();
		//���·�������᷵�������ַ�
		//�Ϸ���������η��أ�224 72
		//�·���������η��أ�224 80
		//����������η��أ�224 75
		//�ҷ���������η��أ�224 77
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
		//��ת
		rotate();
		update = true;
	}

	if (dx != 0) {
		//�����ƶ�
		moveLeftRight(dx);
		update = true;
	}
}

void Tetris::updateWindow()
{
	putimage(0, 0, &imgBg);//���Ʊ���ͼƬ

	//���Է���
	//Block block;
	//block.draw(leftMargin,topMargin);
	IMAGE** imgs = Block::getImages();
	BeginBatchDraw();//���⻭����˸����������һ������Ⱦ
	for (int i = 0; i < rows; i++){
		for (int j = 0; j < cols; j++) {
			if (map[i][j] == 0)continue;

			int x = j * blockSize + leftMargin;
			int y = i * blockSize + topMargin;
			putimage(x, y, imgs[map[i][j]-1]);//��һ

		}
	}


	curBlock->draw(leftMargin, topMargin);
	nextBlock->draw(689,150);

	drawScore();

	EndBatchDraw();
}

//��һ�ε��÷���0��
//���ؾ�����һ�ε��ã�����˶���ms
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

	//�½�������,Υ��λ��
	if (!curBlock->blockInMap(map)) {
		//�̻�����
		bakBlock.solidify(map);
		delay = SPEED_NORMAL[level-1];//���ٹ̻�֮��ָ��ٶ�
		delete curBlock;
		curBlock = nextBlock;
		nextBlock = new Block;

		//�����Ϸ����
		checkOver();
	}
	//���delay = SPEED_NORMAL���ڴ˴�ʵ�ְ�ס�¼����٣��ɿ���ԭ
}

void Tetris::clearLine()
{
	int lines = 0;
	int k = rows - 1;//�洢���ݵ�����
	for (int i = rows - 1; i >= 0; i--) {
		//����i���Ƿ�����
		int count = 0;
		for (int j = 0; j < cols; j++) {
			if (map[i][j]) {
				count++;
			}
			map[k][j] = map[i][j];
			//һ��ɨ��һ�ߴ洢,���еĻ�k���䣬�ϲ㸲�ǵ�ԭk��

		}

		if (count < cols) {//������
			k--;
		}
		else {//count==cols,����
			lines++;
		}
	}
	lineCount += lines;

	if (lines > 0) {
		//�÷�
		int addScore[4] = { 10,30,60,80 };
		score += addScore[lines - 1];

		mciSendString("play res/xiaochu1.mp3", 0, 0, 0);
		update = true;

		//ÿһ�ٷ�Ϊһ��
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
	if (curBlock->getBlockType() == 7) return;//������û��Ҫת

	bakBlock = *curBlock;
	curBlock->rotate();
	if (!curBlock->blockInMap(map)) {
		*curBlock = bakBlock;
	}
}

void Tetris::drawScore()
{
	char scoreText[32];
	sprintf_s(scoreText, sizeof(scoreText), "%d", score);//��ʽ�����Ϊ�ַ���

	setcolor(RGB(180, 180, 180));
	LOGFONT f;
	gettextstyle(&f);//��ȡ��ǰ����
	f.lfHeight = 60;
	f.lfWidth = 30;
	f.lfQuality = ANTIALIASED_QUALITY;//��������Ϊ������ݡ���
	strcpy_s(f.lfFaceName, sizeof(f.lfFaceName), _T("Segoe UI Black"));
	settextstyle(&f);

	setbkmode(TRANSPARENT);//��������͸��

	outtextxy(670,727,scoreText);//���Ʒ���

	sprintf_s(scoreText, sizeof(scoreText), "%d", lineCount);
	gettextstyle(&f);//������������ʾ�Ҷ���
	int xPos = 224 - f.lfWidth * strlen(scoreText);
	outtextxy(xPos, 817, scoreText);//������������

	sprintf_s(scoreText, sizeof(scoreText), "%d", level);
	outtextxy(194, 727, scoreText);//���Ƶڼ���

	sprintf_s(scoreText, sizeof(scoreText), "%d", highestScore);
	outtextxy(670, 817, scoreText);//������߷�
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

	//ʤ����������ʧ�ܽ���
	if (level <= MAX_LEVEL) {
		putimage(262, 361, &imgOver);
		mciSendString("play res/over.mp3", 0, 0, 0);
	}
	else {
		putimage(262, 361, &imgWin);
		mciSendString("play res/win.mp3", 0, 0, 0);
	}
}
