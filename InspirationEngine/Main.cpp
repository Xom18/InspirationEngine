#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MAP_SIZE 30
#define TILE_SIZE 4
#include <SDL\SDL.h>
#include <thread>
#include <mutex>
#include <vector>
#include "MacroDefine.h"
#include "Input.h"
#include "Display.h"
#include "Image.h"
#include "File.h"
#include "Core.h"

int main(int argc, char* argv[])
{
	SDL_Event		SDL_Event;		//창 이벤트처리
	std::thread*	InputThread;	//키보드 입력 스레드, 아직 스레드 화 하기 전
	cDisplay		Display("Inspiration", SCREEN_WIDTH, SCREEN_HEIGHT, 2);	//창 생성
	
	cImage TileSet; // 타일셋이 있는 이미지
	cTile Tile;		// 타일

	cInput Input;	//키보드 입력
	Input.start();

	//타일 이미지셋 불러와서 타일 정의
	TileSet.readBMP("../Release/file.bmp");
	Tile.imageToTile(&TileSet, 16);
	
	SDL_SetTextureBlendMode(Display.getLayer(1), SDL_BLENDMODE_BLEND);

	while (!Display.closed())
	{
		SDL_Rect Rect;//타일 띄울 위치
		Rect.h = 16;
		Rect.w = 16;
		Rect.x = 1 * 16;
		Rect.y = 1 * 16;

		//레이어 텍스쳐 위에 그리기
		SDL_UpdateTexture(Display.getLayer(0), &Rect, Tile.getData(12), 16 * sizeof(Uint32));

		SDL_WaitEventTimeout(&SDL_Event, 30);
		
		Display.render();
	}
	SDL_Quit();
	return 0;
}