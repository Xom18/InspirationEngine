#include "../InspirationEngine/InspirationEngine.h"
#include "MainWindow.h"
#include "DebugWindow.h"

#ifdef _WIN64
	#ifdef _DEBUG
		#pragma comment(lib, "../x64/Debug/InspirationEngine.lib")
	#else
		#pragma comment(lib, "../x64/Release/InspirationEngine.lib")
	#endif
#else
	#ifdef _DEBUG
		#pragma comment(lib, "../Debug/InspirationEngine.lib")
	#else
		#pragma comment(lib, "../Release/InspirationEngine.lib")
	#endif
#endif

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int main(int argc, char* argv[])
{
	InspirationEngine Engine;
	int iTickRate = 16;

	//디버그용 창 좌표
	int iX = 0;
	int iY = 0;
	int iW = 0;
	int iH = 0;

	//주 게임 창 생성
	{
		cMainWindow* lpMainWindow = new cMainWindow();
		bool bSuccess = Engine.addNewWindow("Main", lpMainWindow);

		//실패
		if(!bSuccess)
		{
			delete lpMainWindow;
			return __LINE__;
		}
		Engine.setMainWindow(lpMainWindow);

		lpMainWindow->createWindow(&Engine, "Inspiration", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 2);	//창 생성
		Engine.addWindowIndex(lpMainWindow);

		SDL_Window* lpSDLWindow = lpMainWindow->getSDLWindow();
		SDL_GetWindowPosition(lpSDLWindow, &iX, &iY);
		SDL_GetWindowSize(lpSDLWindow, &iW, &iH);

		lpMainWindow->setRendererLogicalSize(0, 256, 256);
		lpMainWindow->setRendererLogicalSize(1, 512, 512);
	}

	//디버그용 창 생성
	{
		cDebugWindow* lpDebugWindow = new cDebugWindow();
		bool bSuccess = Engine.addNewWindow("Debug", lpDebugWindow);

		//실패
		if(!bSuccess)
		{
			delete lpDebugWindow;
			return __LINE__;
		}

		lpDebugWindow->createWindow(&Engine, "Debugger", 600, 800, iX + iW, iY, 0, 2);	//창 생성
		Engine.addWindowIndex(lpDebugWindow);
	}

	//처리 간격 설정
	Engine.setTickRate(iTickRate);

	//엔진 시작
	Engine.beginEngine();

	//키 입력 등은 여기서 받음
	while (Engine.isRunning())
	{
		SDL_Event Event;
		if(SDL_WaitEventTimeout(&Event, iTickRate))
		{
			Engine.eventPushBack(&Event);
			
			//나머지 이벤트가 있을 수 있으니 처리
			if(SDL_PollEvent(&Event))
				Engine.eventPushBack(&Event);
		}
		
	}
	SDL_Quit();
	return 0;
}