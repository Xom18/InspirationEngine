#if defined(_WIN32) && defined(_DEBUG)
#include <windows.h>
#endif
#include "../InspirationEngine/InspirationEngine.h"
#include "MainWindow.h"
#include "DebugWindow.h"
#include "GameScene.h"
#include "Test/TestStrUTF8.h"

#ifdef _DEBUG
#pragma comment(lib, "../x64/Debug/InspirationEngine.lib")
#else
#pragma comment(lib, "../x64/Release/InspirationEngine.lib")
#endif

#ifdef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#endif

constexpr int32_t g_ScreenWidth = 1920;
constexpr int32_t g_ScreenHeight = 1080;

int main(int argc, char* argv[])
{
#if defined(_WIN32) && defined(_DEBUG)
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
#endif

	// exe 위치(x64/Debug/) 기준으로 Example/ 폴더를 워킹 디렉토리로 고정
	// → "../Data/" 등 상대 경로가 항상 올바르게 해석됨
	if (char* bp = SDL_GetBasePath())
	{
		std::string wd = std::string(bp) + "..\\..\\Example";
		SetCurrentDirectoryA(wd.c_str());
		SDL_free(bp);
	}

	TestStrUTF8().run();

	int32_t iTickRate = 1000 / 60;;

	//디버그용 창 좌표
	int32_t iX = 0;
	int32_t iY = 0;
	int32_t iW = 0;
	int32_t iH = 0;

	//IME출력설정, 기본 텍스트 인풋모드인거 off
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

	//주 게임 창 생성
	{
		MainWindow* lpMainWindow = new MainWindow();
		bool bSuccess = IECore::addNewWindow("Main", lpMainWindow);

		//실패
		if (!bSuccess)
		{
			delete lpMainWindow;
			return __LINE__;
		}
		IECore::setMainWindow(lpMainWindow);

		lpMainWindow->createWindow("Inspiration", g_ScreenWidth, g_ScreenHeight, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 2);	//창 생성

		IECore::addWindowIndex(lpMainWindow);

		SDL_Window* lpSDLWindow = lpMainWindow->getSDLWindow();
		SDL_GetWindowPosition(lpSDLWindow, &iX, &iY);
		SDL_GetWindowSize(lpSDLWindow, &iW, &iH);

	}


	//디버그용 창 생성
	if (true)
	{
		DebugWindow* lpDebugWindow = new DebugWindow();

		bool bSuccess = IECore::addNewWindow("Debug", lpDebugWindow);

		//실패
		if (!bSuccess)
		{
			delete lpDebugWindow;
			return __LINE__;
		}

		lpDebugWindow->createWindow("Debugger", 600, 800, iX + iW, iY, 0, 2);	//창 생성
		IECore::addWindowIndex(lpDebugWindow);

	}

	//처리 간격 설정
	IECore::setTickRate(iTickRate);

	//폰트 추가
	IECore::m_Font.addNewFont(0, "../data/H2PORL.ttf", 20);

	//디버그 창 설정
	DebugWindow* lpDebugWindow = dynamic_cast<DebugWindow*>(IECore::getWindow("Debug"));
	lpDebugWindow->initWindow();

	//테스트 씬 푸시
	IECore::m_Scene.Push(new GameScene());

	//엔진 시작
	IECore::beginEngine();

	bool useIME = false;
	//이벤트 처리루프
	while (IECore::isRunning())
	{
		SDL_Event Event;
		if (SDL_WaitEventTimeout(&Event, iTickRate))
		{
			IECore::eventPushBack(&Event);

			//나머지 이벤트가 있을 수 있으니 처리
			if (SDL_PollEvent(&Event))
				IECore::eventPushBack(&Event);
		}

		//메인스레드에서 호출해야 IME 창 위치 설정하는게 먹음
		//스톱 시키고 바로 스타트 시키는걸 해야 IME 입력박스 위치를 갱신가능
		auto rect = IECore::getTextEditPosition();
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
	IECore::endEngine();

	return 0;
}
