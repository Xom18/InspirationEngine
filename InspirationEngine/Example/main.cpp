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

#ifdef _DEBUG
	#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#endif

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int main(int argc, char* argv[])
{
	std::locale::global(std::locale("ko_KR.UTF-8"));


	int iTickRate = 1000 / 60;;

	//디버그용 창 좌표
	int iX = 0;
	int iY = 0;
	int iW = 0;
	int iH = 0;

	//IME출력설정, 기본 텍스트 인풋모드인거 off
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

	//주 게임 창 생성
	{
		cMainWindow* lpMainWindow = new cMainWindow();
		bool bSuccess = cIECore::addNewWindow("Main", lpMainWindow);

		//실패
		if(!bSuccess)
		{
			delete lpMainWindow;
			return __LINE__;
		}
		cIECore::setMainWindow(lpMainWindow);

		lpMainWindow->createWindow("Inspiration", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 2);	//창 생성

		cIECore::addWindowIndex(lpMainWindow);

		SDL_Window* lpSDLWindow = lpMainWindow->getSDLWindow();
		SDL_GetWindowPosition(lpSDLWindow, &iX, &iY);
		SDL_GetWindowSize(lpSDLWindow, &iW, &iH);

		lpMainWindow->setRendererLogicalSize(0, 64, 64);
		lpMainWindow->setRendererLogicalSize(1, 512, 512);

	}


	//디버그용 창 생성
	if(true)
	{
		cDebugWindow* lpDebugWindow = new cDebugWindow();

		bool bSuccess = cIECore::addNewWindow("Debug", lpDebugWindow);

		//실패
		if(!bSuccess)
		{
			delete lpDebugWindow;
			return __LINE__;
		}

		lpDebugWindow->createWindow("Debugger", 600, 800, iX + iW, iY, 0, 2);	//창 생성
		cIECore::addWindowIndex(lpDebugWindow);

	}

	//처리 간격 설정
	cIECore::setTickRate(iTickRate);

	//폰트 엔진 초기화
	if(TTF_Init() != 0)
		return false;

	//폰트 추가
	cIECore::m_Font.addNewFont(0, "../data/H2PORL.ttf", 20);

	//디버그 창 설정
	cDebugWindow* lpDebugWindow = dynamic_cast<cDebugWindow*>(cIECore::getWindow("Debug"));
	lpDebugWindow->initWindow();

	//엔진 시작
	cIECore::beginEngine();

	//SDL_StopTextInput();
	//SDL_StartTextInput();
	bool useIME = false;
	//이벤트 처리루프
	while (cIECore::isRunning())
	{
		SDL_Event Event;
		if(SDL_WaitEventTimeout(&Event, iTickRate))
		{
			cIECore::eventPushBack(&Event);
			
			//나머지 이벤트가 있을 수 있으니 처리
			if (SDL_PollEvent(&Event))
				cIECore::eventPushBack(&Event);
		}
		
		//메인스레드에서 호출해야 IME 창 위치 설정하는게 먹음
		//스톱 시키고 바로 스타트 시키는건 안할거다 일단 테스트 코드로 남겨둔부분
		auto rect = cIECore::getTextEditPosition();
		if (rect.has_value())
		{
			SDL_SetTextInputRect(&rect.value());
			if (!useIME)
			{
				useIME = true;
				SDL_StopTextInput();
				SDL_StartTextInput();
			}
		}
		else
		{
			if (useIME)
			{
				useIME = false;
				SDL_StopTextInput();
				SDL_StartTextInput();
			}
		}
	}

	//엔진 종료
	cIECore::endEngine();

	return 0;
}