/*步骤：
* 1.设计类：方块Block、Tetris游戏
* 2.设计模块接口
* 3.启动游戏

*/

#include "Tetris.h"
//define CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  

int main()
{
	//_CrtSetBreakAlloc(166);//括号第几次分配内存时泄露
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