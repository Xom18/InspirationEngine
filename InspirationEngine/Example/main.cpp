#ifdef _WIN32
#include <windows.h>
#endif
#include "../InspirationEngine/InspirationEngine.h"
#include "MainWindow.h"
#include "DebugWindow.h"
#include "GameScene.h"
#include "Test/TestStrUTF8.h"

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

	// IME 후보창을 OS 네이티브 UI로 표시 (일본어 등 CJK 입력 힌트)
	SDL_SetHint(SDL_HINT_IME_IMPLEMENTED_UI, "1");

	// exe 위치(Binary/) 기준으로 워킹 디렉토리 고정
	// → "Data/" 등 상대 경로가 항상 올바르게 해석됨
	if (const char* bp = SDL_GetBasePath())
	{
		SetCurrentDirectoryA(bp);
		SDL_free(const_cast<char*>(bp));
	}

	TestStrUTF8().run();

	int32_t iTickRate = 1000 / 60;;

	//디버그용 창 좌표
	int32_t iX = 0;
	int32_t iY = 0;
	int32_t iW = 0;
	int32_t iH = 0;

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
	IECore::GetFont().addNewFont(0, "Data/H2PORL.TTF", 20);

	//디버그 창 설정
	DebugWindow* lpDebugWindow = dynamic_cast<DebugWindow*>(IECore::getWindow("Debug"));
	lpDebugWindow->initWindow();

	//테스트 씬 푸시
	IECore::GetScene().Push(new GameScene());

	//엔진 시작
	IECore::beginEngine();

	//이벤트 처리루프
	SDL_Window* lastFocusWin = nullptr;

	while (IECore::isRunning())
	{
		SDL_Event Event;
		if (SDL_WaitEventTimeout(&Event, iTickRate))
		{
			IECore::eventPushBack(&Event);

			//큐에 남은 이벤트 전부 처리 (TEXT_EDITING 누락 방지)
			while (SDL_PollEvent(&Event))
				IECore::eventPushBack(&Event);
		}

		//포커스된 창에 텍스트 입력 활성화 (창 바뀔 때만 stop/start)
		SDL_Window* focusWin = SDL_GetKeyboardFocus();
		if (focusWin != lastFocusWin)
		{
			if (lastFocusWin)
				SDL_StopTextInput(lastFocusWin);
			if (focusWin)
				SDL_StartTextInput(focusWin);
			lastFocusWin = focusWin;
		}

		//메인스레드에서 호출해야 IME 후보창 위치 설정이 반영됨
		auto rect = IECore::getTextEditPosition();
		if (rect.has_value() && focusWin)
			SDL_SetTextInputArea(focusWin, &rect.value(), 0);

		//SDL3가 Windows에서 TEXT_EDITING을 생성하지 않으므로 플랫폼 폴링으로 대체
		IECore::pollPlatformIME(focusWin);
	}

	//엔진 종료
	IECore::endEngine();

	return 0;
}
