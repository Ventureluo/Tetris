/*���裺
* 1.����ࣺ����Block��Tetris��Ϸ
* 2.���ģ��ӿ�
* 3.������Ϸ

*/

#include "Tetris.h"
//define CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  

int main()
{
	//_CrtSetBreakAlloc(166);//���ŵڼ��η����ڴ�ʱй¶
	//char* pChars = new char[10];
	Tetris game(20, 10, 263, 133, 36);
	game.play();
	_CrtDumpMemoryLeaks();
	return 0;
}



//int main() {
	//Tetris game(20, 10, 263, 133, 36);
	//game.play();
	//return 0;
//}