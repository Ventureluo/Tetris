#pragma once
#include <vector>
#include <graphics.h>
#include "Block.h"
using namespace std;
class Tetris
{
public:
	//���캯��,��Ϸ���У��߿򣬷����С
	Tetris(int rows,int cols,int left,int top,int blockSize);
	void init();//��ʼ��
	void play();//��ʼ��Ϸ

private:
	void keyEvent();
	void updateWindow();
	//������һ�ε��øú�������˶���ʱ�䣨ms��
	//��һ�ε��÷���0
	int getDelay();
	void drop();
	void clearLine();
	void moveLeftRight(int offset);//�����ƶ�
	void rotate();//��ת
	void drawScore();//���Ƶ�ǰ����
	void checkOver();//�����Ϸ�Ƿ����
	void saveScore();//������߷�
	void displayOver();//������Ϸ��������

private:
	int delay;
	bool update;//�Ƿ����

	//����ͼ
	vector<vector<int>> map;
	int rows;
	int cols;
	int leftMargin;
	int topMargin;
	int blockSize;
	IMAGE imgBg;

	//��ǰ�����Ԥ�淽��
	Block* curBlock;
	Block* nextBlock;
	Block bakBlock;//��ǰ���齵������У�����������һ���Ϸ�λ�õ�

	int score;//��ǰ����
	int highestScore;//���
	int level;//��ǰ�ؿ�
	int lineCount;//��������
	bool gameOver;//��Ϸ�Ƿ����

	IMAGE imgOver;
	IMAGE imgWin;
};

