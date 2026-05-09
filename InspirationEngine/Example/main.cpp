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

	int32_t tickRate = 1000 / 60;;

	//디버그용 창 좌표
	int32_t x = 0;
	int32_t y = 0;
	int32_t w = 0;
	int32_t h = 0;

	//주 게임 창 생성
	{
		MainWindow* mainWindow = new MainWindow();
		bool success = IECore::AddNewWindow("Main", mainWindow);

		//실패
		if (!success)
		{
			delete mainWindow;
			return __LINE__;
		}
		IECore::SetMainWindow(mainWindow);

		mainWindow->CreateWindow("Inspiration", g_ScreenWidth, g_ScreenHeight, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 2);	//창 생성

		IECore::AddWindowIndex(mainWindow);

		SDL_Window* sdlWindow = mainWindow->GetSDLWindow();
		SDL_GetWindowPosition(sdlWindow, &x, &y);
		SDL_GetWindowSize(sdlWindow, &w, &h);

	}


	//디버그용 창 생성
	if (true)
	{
		DebugWindow* debugWindow = new DebugWindow();

		bool success = IECore::AddNewWindow("Debug", debugWindow);

		//실패
		if (!success)
		{
			delete debugWindow;
			return __LINE__;
		}

		debugWindow->CreateWindow("Debugger", 600, 800, x + w, y, 0, 2);	//창 생성
		IECore::AddWindowIndex(debugWindow);

	}

	//처리 간격 설정
	IECore::SetTickRate(tickRate);

	//폰트 추가
	IECore::GetFont().AddNewFont(0, "Data/H2PORL.TTF", 20);

	//디버그 창 설정
	DebugWindow* debugWindow = dynamic_cast<DebugWindow*>(IECore::GetWindow("Debug"));
	debugWindow->InitWindow();

	//테스트 씬 푸시
	IECore::GetScene().Push(new GameScene());

	//엔진 시작
	IECore::BeginEngine();

	//이벤트 처리루프
	SDL_Window* lastFocusWin = nullptr;

	while (IECore::IsRunning())
	{
		SDL_Event Event;
		if (SDL_WaitEventTimeout(&Event, tickRate))
		{
			IECore::EventPushBack(&Event);

			//큐에 남은 이벤트 전부 처리 (TEXT_EDITING 누락 방지)
			while (SDL_PollEvent(&Event))
				IECore::EventPushBack(&Event);
		}

		//포커스된 창에 텍스트 입력 활성화 (창 바뀔 때만 stop/start)
		SDL_Window* focusWin = SDL_GetKeyboardFocus();
		if (focusWin != lastFocusWin)
		{
			if (lastFocusWin != nullptr)
				SDL_StopTextInput(lastFocusWin);
			if (focusWin != nullptr)
				SDL_StartTextInput(focusWin);
			lastFocusWin = focusWin;
		}

		//메인스레드에서 호출해야 IME 후보창 위치 설정이 반영됨
		auto rect = IECore::GetTextEditPosition();
		if (rect.has_value() && focusWin != nullptr)
			SDL_SetTextInputArea(focusWin, &rect.value(), 0);

		//SDL3가 Windows에서 TEXT_EDITING을 생성하지 않으므로 플랫폼 폴링으로 대체
		IECore::PollPlatformIME(focusWin);
	}

	//엔진 종료
	IECore::EndEngine();

	return 0;
}
