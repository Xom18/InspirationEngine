#include <future>
#include <iostream>
#include "InspirationEngine.h"
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


void IECore::mainThread()
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
		operateEvent();

		m_operatePhase = EnginePhase::Update;
		update(m_deltaTime);

		m_operatePhase = EnginePhase::Draw;
		draw();

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

void IECore::operateEvent()
{
	//큐에서 이벤트 가져옴
	std::deque<SDL_Event> dqEventQueue;
	m_eventMutex.lock();
	std::swap(dqEventQueue, m_eventQueue);
	m_eventMutex.unlock();


	//이벤트 처리
	while (!dqEventQueue.empty())
	{
		SDL_Event Event = dqEventQueue.front();
		dqEventQueue.pop_front();

		if (operateTextEdit(&Event))
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
				m_textEditPosition.x = m_focusedTextBox->getCursurScreenPos().x;
				m_textEditPosition.y = m_focusedTextBox->getCursurScreenPos().y;
				m_textEditPosition.w = 0;
				m_textEditPosition.h = m_textEditPosition.y + m_focusedTextBox->getFontHeight();
			}
			m_textEditMutex.unlock();
		}
		break;
		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_KEY_UP:
		{
			m_Input.setKeyState(Event.key.scancode, Event.key.down);
		}
		break;
		case SDL_EVENT_WINDOW_RESIZED:
		{
			IEWindow* lpWindow = getWindowByID(Event.window.windowID);
			if (lpWindow != nullptr)
				lpWindow->resizeRenderer();
		}
		break;
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
		{
			IEWindow* lpWindow = getWindowByID(Event.window.windowID);
			if (lpWindow != nullptr)
				lpWindow->callXButton();
		}
		break;
		}
	}

	//마우스 위치 갱신
	{
		float fMouseX = 0.0f;
		float fMouseY = 0.0f;
		SDL_GetMouseState(&fMouseX, &fMouseY);
		int32_t iMouseX = static_cast<int32_t>(fMouseX);
		int32_t iMouseY = static_cast<int32_t>(fMouseY);
		m_Input.updateMousePos(iMouseX, iMouseY);

		//마우스가 올라와있는 윈도우
		SDL_Window* lpWindow = SDL_GetMouseFocus();
		if (lpWindow)
		{
			Uint32 uiWindowID = SDL_GetWindowID(lpWindow);
			m_mouseOnWindow = getWindowByID(uiWindowID);
		}
		else
		{
			m_mouseOnWindow = nullptr;
		}
	}
}


void IECore::update(float deltaTime)
{
	for (auto& [_, window] : m_windows)
		window->update(deltaTime);
}

void IECore::draw()
{
	auto ite = m_windows.begin();
	//그릴게 없다
	if (ite == m_windows.end())
		return;

	//드로우 한 기록 초기화
	m_drawCompleteCounter.store(0);
	for (; ite != m_windows.end(); ++ite)
		ite->second->resetDrawed();

	//각 창 스레드에 그리라고 전달
	m_drawThreadWaiter.notify_all();

	std::mutex mtxWaiter;
	std::unique_lock<std::mutex> lkWaiter(mtxWaiter);

	//그리기 완료 때 까지 대기 / 엔진이 멈추면 얘도 멈춰줘야된다
	m_drawCompleteWaiter.wait(lkWaiter, [&] {
		return isDrawComplete() || !isRunning();
		});
}

bool IECore::beginEngine()
{
	if (m_mainThread)
		return false;

	m_isRunning = true;

	m_mainThread = std::make_unique<std::thread>([&]() { mainThread(); });

	//스레드를 각자 파서 각자 그리도록
	for (auto& [_, window] : m_windows)
		window->beginDrawThread();

	return true;
}

bool IECore::endEngine()
{
	if (m_mainThread)
	{
		m_mainThread->join();
		m_mainThread.reset();
	}

	for (auto& [_, window] : m_windows)//각 창 삭제
		window->close();

	SDL_Quit();
	return true;
}

bool IECore::operateTextEdit(SDL_Event* event)
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
				&& m_focusedTextBox->getTextLength() > 0)
				m_focusedTextBox->removeByBackspace();

			if (event->key.key == SDLK_DELETE
				&& m_focusedTextBox->getTextLength() > 0)
				m_focusedTextBox->removeByDelete();

			//커서 좌우로 이동
			if (event->key.key == SDLK_RIGHT)
				m_focusedTextBox->cusorMoveNext();
			if (event->key.key == SDLK_LEFT)
				m_focusedTextBox->cusorMovePrevious();

			//엔터
			if (event->key.key == SDLK_RETURN
				|| event->key.key == SDLK_KP_ENTER)
				m_focusedTextBox->insertCusorPos("\n");
		}
	}
	break;
	case SDL_EVENT_TEXT_EDITING:
	{//조합형 입력중
		if (strlen(event->text.text) == 0)
		{
			if (m_useIME)
				m_focusedTextBox->removeIMEInput();
			m_useIME = false;
			break;
		}

		if (m_useIME)
			m_focusedTextBox->removeIMEInput();

		m_focusedTextBox->insertCusorPos(event->text.text);
		m_focusedTextBox->setIMELength(strlen(event->text.text));
		m_useIME = true;
	}
	break;
	case SDL_EVENT_TEXT_INPUT:
	{//입력
		if (m_useIME)
			m_focusedTextBox->removeIMEInput();

		m_useIME = false;
		m_focusedTextBox->insertCusorPos(event->text.text);
	}
	break;
	default:
		return false;
		break;
	}

	return true;
}

std::optional<SDL_Rect> IECore::getTextEditPosition()
{
	std::lock_guard lockGuard(m_textEditMutex);
	if (!m_textEdit)
		return std::nullopt;

	return m_textEditPosition;
}
