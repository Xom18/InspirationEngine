#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MAP_SIZE 30
#define TILE_SIZE 4

#include "InspirationEngine.h"

int main(int argc, char* argv[])
{
	SDL_Event		SDL_Event;		//창 이벤트처리
	std::thread*	InputThread;	//키보드 입력 스레드, 아직 스레드 화 하기 전
	cDisplay		Display;
	
	Display.createWindow("Inspiration", SCREEN_WIDTH, SCREEN_HEIGHT, 2);	//창 생성
	
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

	Display.setRendererLogicalSize(0, 256, 256);
//	Display.setRendererLogicalSize(1, 512, 512);

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
		if(keytes[SDL_SCANCODE_ESCAPE])
		{
			Display.close();
			break;
		}

		Display.draw(aTile, 1, iPosX + 10, iPosY, 32, 64);
		Display.draw(aTile, 0, iPosX, iPosY, 32, 64);

		SDL_WaitEventTimeout(&SDL_Event, 30);
		if(SDL_Event.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			SDL_Window* win = SDL_GetWindowFromID(SDL_Event.window.windowID);
			if(win == Display.getWindow())
				Display.resize();
		}
		
		Display.render();
	}
	SDL_Quit();
	return 0;
}