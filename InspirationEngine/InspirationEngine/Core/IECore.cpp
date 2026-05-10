#include <future>
#include <iostream>
#include "InspirationEngine.h"

// SDL3가 Windows TEXT_EDITING 이벤트를 생성하지 않는 동안 직접 IME 폴링
// SDL3가 이 문제를 수정하면 아래 define을 제거하면 됨
#define _CUSTOM_IME

#if defined(_WIN32) && defined(_CUSTOM_IME)
#include <windows.h>
#include <imm.h>
#pragma comment(lib, "imm32.lib")
#endif
//public
IEInput			IECore::m_Input;						//입력, 클릭이나 창 내부 처리도 여기서 받은다음 각 창으로 보냄
IEDebugInfo		IECore::m_DebugInfo;					//디버그 툴
IEFontManager	IECore::m_Font;							//폰트 관리하는곳
IESpriteManager	IECore::m_Sprite;
IEAtlasManager	IECore::m_Atlas;
IESceneManager	IECore::m_Scene;
float			IECore::m_deltaTime   = 0.0f;
uint64_t		IECore::m_deltaTimeMs = 0;						//스프라이트 관리하는곳
IEWindow* IECore::m_mainWindow = nullptr;			//메인 윈도우
IEWindow* IECore::m_mouseOnWindow = nullptr;		//마우스가 올라가 있는 윈도우
IEWindow* IECore::m_focusedWindow = nullptr;		//선택 되있는 윈도우
IETextBox* IECore::m_focusedTextBox = nullptr;		//선택 되있는 윈도우

//private
EnginePhase		IECore::m_operatePhase = EnginePhase::NaN;
std::mutex		IECore::m_eventMutex;					//이벤트 뮤텍스

std::map<std::string, std::unique_ptr<IEWindow>>	IECore::m_windows;	//윈도우
std::map<Uint32, IEWindow*>	IECore::m_windowsByID;					//윈도우
std::unique_ptr<std::thread> IECore::m_mainThread;					//메인 스레드
int32_t			IECore::m_tickRate = 16;				//메인스레드 처리간격(ms)
bool			IECore::m_isRunning = false;			//구동중 여부
std::deque<SDL_Event> IECore::m_eventQueue;				//처리 안한 이벤트 큐

std::atomic<int32_t>	IECore::m_drawCompleteCounter;	//그리기 완료 카운터
std::condition_variable IECore::m_drawThreadWaiter;		//각 창의 drawthread를 대기 시켜주는곳
std::condition_variable IECore::m_drawCompleteWaiter;	//각 창의 drawthread를 대기 시켜주는곳

bool IECore::m_useIME = false;
std::mutex IECore::m_textEditMutex;						// IME 사용 처리를 위한 뮤텍스
bool IECore::m_textEdit = false;						// 텍스트 편집 사용중인지
SDL_Rect IECore::m_textEditPosition;					// 텍스트 편집 사용중일때 커서 위치

std::string IECore::m_pendingIMEComposition;
bool IECore::m_imeCompositionDirty = false;


void IECore::MainThread()
{
	auto tickCycle = std::chrono::milliseconds(m_tickRate);
	auto lastTime  = std::chrono::steady_clock::now();
	std::chrono::system_clock::time_point StartTime = std::chrono::system_clock::now();	//시작시간
	std::chrono::system_clock::time_point NextTime = StartTime + tickCycle;				//다음틱

	while (m_isRunning)
	{
		auto now     = std::chrono::steady_clock::now();
		auto elapsed = now - lastTime;
		m_deltaTime   = std::chrono::duration<float>(elapsed).count();
		m_deltaTimeMs = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count());
		lastTime = now;

		auto TargetNextTime = NextTime;	//목표 틱
		NextTime += tickCycle;			//다음 목표틱

		m_operatePhase = EnginePhase::OperateEvent;
		OperateEvent();

		m_operatePhase = EnginePhase::Update;
		Update(m_deltaTime);

		m_operatePhase = EnginePhase::Draw;
		Draw();

		m_operatePhase = EnginePhase::Complete;

		if (TargetNextTime < std::chrono::system_clock::now())
		{
			//처리가 너무 오래걸렸다
			continue;
		}

		//남은 시간만큼 재운다
		std::this_thread::sleep_for(TargetNextTime - std::chrono::system_clock::now());
	}

	return;
}

void IECore::OperateEvent()
{
	//큐에서 이벤트 가져옴 + pending IME composition 수거
	std::deque<SDL_Event> eventQueue;
	std::string pendingIME;
	bool imeDirty = false;
	{
		std::lock_guard<std::mutex> lock(m_eventMutex);
		std::swap(eventQueue, m_eventQueue);
		if (m_imeCompositionDirty)
		{
			pendingIME = std::move(m_pendingIMEComposition);
			m_pendingIMEComposition.clear();
			m_imeCompositionDirty = false;
			imeDirty = true;
		}
	}

	//이벤트 처리 (TEXT_INPUT 커밋이 먼저 반영돼야 pendingIME가 올바르게 이어짐)
	while (!eventQueue.empty())
	{
		SDL_Event Event = eventQueue.front();
		eventQueue.pop_front();

		if (OperateTextEdit(&Event))
			continue;

		switch (Event.type)
		{
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			//테스트용 코드
			m_textEditMutex.lock();
			m_textEdit = true;
			if (m_focusedTextBox != nullptr)
			{
				m_textEditPosition.x = m_focusedTextBox->GetCursorScreenPos().x;
				m_textEditPosition.y = m_focusedTextBox->GetCursorScreenPos().y;
				m_textEditPosition.w = 0;
				m_textEditPosition.h = m_focusedTextBox->GetFontHeight();
			}
			m_textEditMutex.unlock();
		}
		break;
		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_KEY_UP:
		{
			m_Input.SetKeyState(Event.key.scancode, Event.key.down);
		}
		break;
		case SDL_EVENT_WINDOW_RESIZED:
		{
			IEWindow* window = GetWindowByID(Event.window.windowID);
			if (window != nullptr)
				window->ResizeRenderer();
		}
		break;
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
		{
			IEWindow* window = GetWindowByID(Event.window.windowID);
			if (window != nullptr)
				window->CallXButton();
		}
		break;
		}
	}

	//Windows IME 폴링 결과 처리 (이벤트 큐 처리 이후 — TEXT_INPUT 커밋 반영 후 새 composition 삽입)
	if (imeDirty && m_focusedTextBox != nullptr && m_textEdit)
	{
		if (pendingIME.empty())
		{
			if (m_useIME)
			{
				m_focusedTextBox->RemoveIMEInput();
				m_useIME = false;
			}
		}
		else
		{
			if (m_useIME)
				m_focusedTextBox->RemoveIMEInput();
			m_focusedTextBox->InsertCursorPos(pendingIME.c_str());
			m_focusedTextBox->SetIMELength(pendingIME.size());
			m_useIME = true;
		}
	}

	//마우스 위치 갱신
	{
		float mouseX = 0.0f;
		float mouseY = 0.0f;
		SDL_GetMouseState(&mouseX, &mouseY);
		int32_t mouseXInt = static_cast<int32_t>(mouseX);
		int32_t mouseYInt = static_cast<int32_t>(mouseY);
		m_Input.UpdateMousePos(mouseXInt, mouseYInt);

		//마우스가 올라와있는 윈도우
		SDL_Window* sdlWindow = SDL_GetMouseFocus();
		if (sdlWindow != nullptr)
		{
			Uint32 windowID = SDL_GetWindowID(sdlWindow);
			m_mouseOnWindow = GetWindowByID(windowID);
		}
		else
		{
			m_mouseOnWindow = nullptr;
		}
	}
}


void IECore::Update(float deltaTime)
{
	for (auto& [_, window] : m_windows)
		window->Update(deltaTime);
}

void IECore::Draw()
{
	auto ite = m_windows.begin();
	//그릴게 없다
	if (ite == m_windows.end())
		return;

	//드로우 한 기록 초기화
	m_drawCompleteCounter.store(0);
	for (; ite != m_windows.end(); ++ite)
		ite->second->ResetDrawed();

	//각 창 스레드에 그리라고 전달
	m_drawThreadWaiter.notify_all();

	std::mutex mtxWaiter;
	std::unique_lock<std::mutex> lkWaiter(mtxWaiter);

	//그리기 완료 때 까지 대기 / 엔진이 멈추면 얘도 멈춰줘야된다
	m_drawCompleteWaiter.wait(lkWaiter, [&] {
		return IsDrawComplete() || !IsRunning();
		});
}

bool IECore::BeginEngine()
{
	if (m_mainThread != nullptr)
		return false;

	m_isRunning = true;

	m_mainThread = std::make_unique<std::thread>([&]() { MainThread(); });

	//스레드를 각자 파서 각자 그리도록
	for (auto& [_, window] : m_windows)
		window->BeginDrawThread();

	return true;
}

bool IECore::EndEngine()
{
	if (m_mainThread != nullptr)
	{
		m_mainThread->join();
		m_mainThread.reset();
	}

	for (auto& [_, window] : m_windows)//각 창 삭제
		window->Close();

	//SDL_Quit();
	return true;
}

bool IECore::OperateTextEdit(SDL_Event* event)
{
	if (m_focusedTextBox == nullptr
		|| !m_textEdit)
		return false;

	switch (event->type)
	{
	case SDL_EVENT_KEY_DOWN:
	{
		if (!m_useIME)
		{
			//텍스트 지우기
			if (event->key.key == SDLK_BACKSPACE
				&& m_focusedTextBox->GetTextLength() > 0)
				m_focusedTextBox->RemoveByBackspace();

			if (event->key.key == SDLK_DELETE
				&& m_focusedTextBox->GetTextLength() > 0)
				m_focusedTextBox->RemoveByDelete();

			//커서 좌우로 이동
			if (event->key.key == SDLK_RIGHT)
				m_focusedTextBox->CursorMoveNext();
			if (event->key.key == SDLK_LEFT)
				m_focusedTextBox->CursorMovePrevious();

			//엔터
			if (event->key.key == SDLK_RETURN
				|| event->key.key == SDLK_KP_ENTER)
				m_focusedTextBox->InsertCursorPos("\n");
		}
	}
	break;
	case SDL_EVENT_TEXT_EDITING:
	{//조합형 입력중
		const char* editText = event->edit.text;
		if (editText == nullptr || strlen(editText) == 0)
		{
			if (m_useIME)
				m_focusedTextBox->RemoveIMEInput();
			m_useIME = false;
			break;
		}

		if (m_useIME)
			m_focusedTextBox->RemoveIMEInput();

		m_focusedTextBox->InsertCursorPos(editText);
		m_focusedTextBox->SetIMELength(strlen(editText));
		m_useIME = true;
	}
	break;
	case SDL_EVENT_TEXT_INPUT:
	{//입력
		if (m_useIME)
			m_focusedTextBox->RemoveIMEInput();

		m_useIME = false;
		m_focusedTextBox->InsertCursorPos(event->text.text);
	}
	break;
	default:
		return false;
		break;
	}

	return true;
}

std::optional<SDL_Rect> IECore::GetTextEditPosition()
{
	std::lock_guard lockGuard(m_textEditMutex);
	if (!m_textEdit)
		return std::nullopt;

	return m_textEditPosition;
}

void IECore::PollPlatformIME(SDL_Window* focusWin)
{
#if defined(_WIN32) && defined(_CUSTOM_IME)
	static std::string s_lastComposition;
	static SDL_Window* s_lastFocusWin = nullptr;

	if (focusWin != s_lastFocusWin)
	{
		if (!s_lastComposition.empty())
		{
			SetIMEComposition("");
			s_lastComposition.clear();
		}
		s_lastFocusWin = focusWin;
	}

	if (focusWin == nullptr)
		return;

	HWND hwnd = (HWND)SDL_GetPointerProperty(
		SDL_GetWindowProperties(focusWin), "SDL.window.win32.hwnd", nullptr);
	if (hwnd == nullptr)
		return;

	HIMC hImc = ImmGetContext(hwnd);
	if (hImc == nullptr)
		return;

	//후보창을 커서 바로 아래/옆에 위치 (CFS_EXCLUDE: 지정 rect와 겹치지 않게 배치)
	{
		SDL_Rect cursorRect = {};
		{
			std::lock_guard<std::mutex> lock(m_textEditMutex);
			cursorRect = m_textEditPosition;
		}
		CANDIDATEFORM cf = {};
		cf.dwIndex        = 0;
		cf.dwStyle        = CFS_EXCLUDE;
		cf.ptCurrentPos   = { cursorRect.x, cursorRect.y + cursorRect.h };
		cf.rcArea         = { cursorRect.x, cursorRect.y,
		                      cursorRect.x + cursorRect.w, cursorRect.y + cursorRect.h };
		ImmSetCandidateWindow(hImc, &cf);
	}

	LONG dwSize = ImmGetCompositionStringW(hImc, GCS_COMPSTR, nullptr, 0);
	if (dwSize > 0)
	{
		std::wstring wcomp(static_cast<size_t>(dwSize) / sizeof(wchar_t), L'\0');
		ImmGetCompositionStringW(hImc, GCS_COMPSTR, wcomp.data(), dwSize);

		int utf8Len = WideCharToMultiByte(CP_UTF8, 0,
			wcomp.c_str(), static_cast<int>(wcomp.size()),
			nullptr, 0, nullptr, nullptr);
		std::string utf8comp(static_cast<size_t>(utf8Len), '\0');
		WideCharToMultiByte(CP_UTF8, 0,
			wcomp.c_str(), static_cast<int>(wcomp.size()),
			utf8comp.data(), utf8Len, nullptr, nullptr);

		if (utf8comp != s_lastComposition)
		{
			s_lastComposition = utf8comp;
			SetIMEComposition(utf8comp);
		}
	}
	else if (!s_lastComposition.empty())
	{
		s_lastComposition.clear();
		SetIMEComposition("");
	}

	ImmReleaseContext(hwnd, hImc);
#endif
}
