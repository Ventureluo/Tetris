#include "Block.h"
#include <stdlib.h>

IMAGE* Block::imgs[7] = { NULL, };
int Block::size = 36;

Block::Block()
{
	if (imgs[0] == NULL) {
		IMAGE imgTmp;
		loadimage(&imgTmp, "res/tiles.png");

		SetWorkingImage(&imgTmp);//ͼ���и�
		for (int i = 0; i < 7; i++) {
			imgs[i] = new IMAGE;
			getimage(imgs[i], i * size, 0, size, size);
		}
		SetWorkingImage();//�ָ�������
	}

	int blocks[7][4] = {
	1,3,5,7,//I
	2,4,5,7,//Z 1��
	3,5,4,6,//Z 2��
	3,5,6,7,//T
	2,3,5,7,//L
	3,5,7,6,//J
	2,3,4,5,//��
	};
	//�������һ�ַ���
	blockType = 1+rand() % 7; //��һ��Ϊ�˷�ֹ����0���ͼ��0���ظ�����������

	//��ʼ��smallBlocks
	for (int i = 0; i < 4; i++) {
		int value = blocks[blockType-1][i];
		smallBlocks[i].row = value / 2;
		smallBlocks[i].col = value % 2;
	}

	img = imgs[blockType-1];
}

void Block::drop()
{
	for (auto &block:smallBlocks) {
		block.row++;
	}
}

void Block::moveLeftRight(int offset)
{
	for (int i = 0; i < 4; i++) {
		smallBlocks[i].col += offset;
	}
}

void Block::rotate()
{
	Point p = smallBlocks[1];//���ĵ�

	for (int i = 0; i < 4; i++) {
		Point tmp = smallBlocks[i];
		smallBlocks[i].col = p.col - tmp.row + p.row;
		smallBlocks[i].row = p.row + tmp.col - p.col;
	}
}

void Block::draw(int leftMargin, int topMargin)
{
	for (int i = 0; i < 4; i++) {
		int x = leftMargin + smallBlocks[i].col * size;
		int y = topMargin + smallBlocks[i].row * size;
		putimage(x, y, img);
	}
}

IMAGE** Block::getImages()
{
	return imgs;
}

Block& Block::operator=(const Block& other)
{
	if (this == &other) return *this;//�Լ���ֵ�Լ�
	this->blockType = other.blockType;
	for (int i = 0; i < 4; i++) {
		this->smallBlocks[i] = other.smallBlocks[i];
	}
	return *this;
}

//��ⷽ���Ƿ��ںϷ�λ��
bool Block::blockInMap(const vector<vector<int>>& map)
{
	int rows = map.size();
	int cols = map[0].size();
	for (int i = 0; i < 4; i++) {
		if (smallBlocks[i].col < 0 || smallBlocks[i].col >= cols || smallBlocks[i].row < 0 ||
			smallBlocks[i].row >= rows || map[smallBlocks[i].row][smallBlocks[i].col]) {
			return false;
		}
	}
}

//�̻���map��
void Block::solidify(vector<vector<int>>& map)
{
	for (int i = 0; i < 4; i++) {
		map[smallBlocks[i].row][smallBlocks[i].col] = blockType;
	}
}

int Block::getBlockType()
{
	return blockType;
}
