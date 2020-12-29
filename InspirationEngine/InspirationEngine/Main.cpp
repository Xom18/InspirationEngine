#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MAP_SIZE 30
#define TILE_SIZE 4

#include "InspirationEngine.h"

int main(int argc, char* argv[])
{
	InspirationEngine Engine;
	
	Engine.m_Display.createWindow("Inspiration", SCREEN_WIDTH, SCREEN_HEIGHT, 0, 2);	//창 생성
	Engine.m_Input.start();

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

	Engine.m_Display.setRendererLogicalSize(0, 256, 256);
	Engine.m_Display.setRendererLogicalSize(1, 512, 512);

	while (!Engine.m_Display.closed())
	{
		SDL_Rect Rect;//타일 띄울 위치
		Rect.h = 32;
		Rect.w = 32;
		Rect.x = iPosX;
		Rect.y = iPosY;

		const Uint8* keytes = Engine.m_Input.getKeyInput();
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
			Engine.m_Display.close();
			break;
		}

		Engine.m_Display.drawBuffer(aTile, 0, 32, 64, 100, 100);
		Engine.m_Display.drawBuffer(aTile, 1, 32, 64, 150, 150);

		SDL_WaitEventTimeout(&Engine.m_Event, 30);
		if(Engine.m_Event.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			SDL_Window* win = SDL_GetWindowFromID(Engine.m_Event.window.windowID);
			if(win == Engine.m_Display.getWindow())
				Engine.m_Display.resizeRenderer();
		}
		
		Engine.m_Display.render();
	}
	SDL_Quit();
	return 0;
}