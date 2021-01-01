//#define SCREEN_WIDTH 1280
//#define SCREEN_HEIGHT 720
//#define MAP_SIZE 30
//#define TILE_SIZE 4
//
//#include "InspirationEngine.h"
//
//#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
//
//int main(int argc, char* argv[])
//{
//	InspirationEngine Engine;
//	int iTickRate = 16;
//
//	//디버그용 창 좌표
//	int iX = 0;
//	int iY = 0;
//	int iW = 0;
//	int iH = 0;
//
//	//주 게임 창 생성
//	{
//		cWindow* lpWindow = Engine.allocWindow("Main");
//		lpWindow->createWindow("Inspiration", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 2);	//창 생성
//		Engine.addWindowIndex(lpWindow);
//
//		SDL_Window* lpSDLWindow = lpWindow->getSDLWindow();
//		SDL_GetWindowPosition(lpSDLWindow, &iX, &iY);
//		SDL_GetWindowSize(lpSDLWindow, &iW, &iH);
//
//		lpWindow->setRendererLogicalSize(0, 256, 256);
//		lpWindow->setRendererLogicalSize(1, 512, 512);
//	}
//
//	//디버그용 창 생성
//	{
//		cWindow* lpWindow = Engine.allocWindow("Debug");
//		lpWindow->createWindow("Debugger", 600, 800, iX + iW, iY, 0, 2);	//창 생성
//		Engine.addWindowIndex(lpWindow);
//		lpWindow->setIsCanClose(true);
//	}
//
//	Engine.setTickRate(iTickRate);
//
//	Engine.beginEngine();
//
//	while (Engine.isRunning())
//	{
//		SDL_Event Event;
//		if(SDL_WaitEventTimeout(&Event, iTickRate))
//		{
//			Engine.eventPushBack(&Event);
//			
//			//나머지 이벤트가 있을 수 있으니 처리
//			if(SDL_PollEvent(&Event))
//				Engine.eventPushBack(&Event);
//		}
//		
//	}
//	SDL_Quit();
//	return 0;
//}