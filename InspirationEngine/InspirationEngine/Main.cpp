#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MAP_SIZE 30
#define TILE_SIZE 4

#include "InspirationEngine.h"

int main(int argc, char* argv[])
{
	SDL_Event		SDL_Event;		//창 이벤트처리
	std::thread*	InputThread;	//키보드 입력 스레드, 아직 스레드 화 하기 전
	cDisplay		Display("Inspiration", SCREEN_WIDTH, SCREEN_HEIGHT);	//창 생성
	
	cInput			Input;			//키보드 입력
	Input.start();

	SDL_Surface* pSurface = IMG_Load("../Release/DungeonCrawl_ProjectUtumnoTileset.png");

	int iPosX = 0;
	int iPosY = 0;

	int aTile[32 * 64];

	int* lpPoint = (int*)pSurface->pixels;
	lpPoint += 32 * 5 + 32 * 4 * pSurface->w;

	for(int i = 0; i < 64; ++i)
	{
		memcpy(aTile + i * 32, lpPoint, sizeof(int) * 32);
		lpPoint += pSurface->w;
	}

	while (!Display.closed())
	{
		SDL_Rect Rect;//타일 띄울 위치
		Rect.h = 32;
		Rect.w = 32;
		Rect.x = iPosX;
		Rect.y = iPosY;

		const Uint8* keytes = Input.getKeyInput();
		if(keytes[SDL_SCANCODE_KP_1])
		{
			iPosX -= 1;
			iPosY += 1;
		}
		if(keytes[SDL_SCANCODE_KP_2])
		{
			iPosY += 1;
		}
		if(keytes[SDL_SCANCODE_KP_3])
		{
			iPosX += 1;
			iPosY += 1;
		}
		if(keytes[SDL_SCANCODE_KP_4])
		{
			iPosX -= 1;
		}
		if(keytes[SDL_SCANCODE_KP_6])
		{
			iPosX += 1;
		}
		if(keytes[SDL_SCANCODE_KP_7])
		{
			iPosX -= 1;
			iPosY -= 1;
		}
		if(keytes[SDL_SCANCODE_KP_8])
		{
			iPosY -= 1;
		}
		if(keytes[SDL_SCANCODE_KP_9])
		{
			iPosX += 1;
			iPosY -= 1;
		}

		Display.draw(aTile, iPosX, iPosY, 32, 64);

		SDL_WaitEventTimeout(&SDL_Event, 30);
		
		Display.render();
	}
	SDL_Quit();
	return 0;
}