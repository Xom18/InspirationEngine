#include <future>
#include "InspirationEngine.h"
#include <iostream>
//public
cInput			cIECore::m_Input;						//입력, 클릭이나 창 내부 처리도 여기서 받은다음 각 창으로 보냄
cDebugInfo		cIECore::m_DebugInfo;					//디버그 툴
cFontManager	cIECore::m_Font;						//폰트 관리하는곳
cWindow* cIECore::m_lpMainWindow = nullptr;		//메인 윈도우
cWindow* cIECore::m_lpMouseOnWindow = nullptr;	//마우스가 올라가 있는 윈도우
cWindow* cIECore::m_lpFocusedWindow = nullptr;	//선택 되있는 윈도우
cTextBox* cIECore::m_lpFocusedTextBox = nullptr;	//선택 되있는 윈도우

//private
int				cIECore::m_iOperatePahse = 0;			//현재 어느걸 처리중인지
std::mutex		cIECore::m_mtxEvent;					//이벤트 뮤텍스

std::map<std::string, cWindow*>	cIECore::m_mapWindow;	//윈도우
std::map<Uint32, cWindow*>	cIECore::m_mapWindowByID;	//윈도우
std::thread* cIECore::m_pMainThread = nullptr;		//메인 스레드
int				cIECore::m_iTickRate = 16;				//메인스레드 처리간격(ms)
bool			cIECore::m_bIsRunning = false;			//구동중 여부
std::deque<SDL_Event> cIECore::m_dqEventQueue;			//처리 안한 이벤트 큐

std::atomic<int>		cIECore::m_iDrawCompleteCounter;//그리기 완료 카운터
std::condition_variable cIECore::m_cvDrawThreadWaiter;	//각 창의 drawthread를 대기 시켜주는곳
std::condition_variable cIECore::m_cvDrawCompleteWaiter;//각 창의 drawthread를 대기 시켜주는곳

bool cIECore::m_bUseIME = false;
std::mutex cIECore::m_mtxTextEdit;					// IME 사용 처리를 위한 뮤텍스
bool cIECore::m_bTextEdit = false;			// 텍스트 편집 사용중인지
SDL_Rect cIECore::m_rtTextEditPosition;			// 텍스트 편집 사용중일때 커서 위치


void cIECore::mainThread()
{
	auto tickCycle = std::chrono::milliseconds(m_iTickRate);
	std::chrono::system_clock::time_point StartTime = std::chrono::system_clock::now();	//시작시간
	std::chrono::system_clock::time_point NextTime = StartTime + tickCycle;				//다음틱

	while (m_bIsRunning)
	{
		auto TargetNextTime = NextTime;	//목표 틱
		NextTime += tickCycle;			//다음 목표틱

		m_iOperatePahse = eENGINE_PAHSE_OPERATE_EVENT;
		operateEvent();

		m_iOperatePahse = eENGINE_PAHSE_UPDATE;
		update();

		m_iOperatePahse = eENGINE_PAHSE_DRAW;
		draw();

		m_iOperatePahse = eENGINE_PAHSE_COMPLETE;

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

void cIECore::operateEvent()
{
	//큐에서 이벤트 가져옴
	std::deque<SDL_Event> dqEventQueue;
	m_mtxEvent.lock();
	std::swap(dqEventQueue, m_dqEventQueue);
	m_mtxEvent.unlock();


	//이벤트 처리
	while (!dqEventQueue.empty())
	{
		SDL_Event Event = dqEventQueue.front();
		dqEventQueue.pop_front();

		if (operateTextEdit(&Event))
			continue;

		switch (Event.type)
		{
		case SDL_EventType::SDL_MOUSEBUTTONDOWN:
		case SDL_EventType::SDL_MOUSEBUTTONUP:
		{
			//테스트용 코드
			m_mtxTextEdit.lock();
			m_bTextEdit = true;
			if(m_lpFocusedTextBox != nullptr)
			{
				m_rtTextEditPosition.x = m_lpFocusedTextBox->getCursurScreenPos().x;
				m_rtTextEditPosition.y = m_lpFocusedTextBox->getCursurScreenPos().y;
				m_rtTextEditPosition.w = 0;
				m_rtTextEditPosition.h = m_rtTextEditPosition.y + m_lpFocusedTextBox->getFontHeight();
			}
			m_mtxTextEdit.unlock();
		}
		break;
		case SDL_EventType::SDL_KEYDOWN:
		case SDL_EventType::SDL_KEYUP:
		{
			m_Input.setKeyState(Event.key.keysym.scancode, Event.key.state);
		}
		break;
		case SDL_EventType::SDL_WINDOWEVENT:
		{
			operateWindowEvent(&Event);
		}
		break;
		}
	}

	//마우스 위치 갱신
	{
		int iMouseX = 0;
		int iMouseY = 0;
		SDL_GetMouseState(&iMouseX, &iMouseY);
		m_Input.updateMousePos(iMouseX, iMouseY);

		//마우스가 올라와있는 윈도우
		SDL_Window* lpWindow = SDL_GetMouseFocus();
		if (lpWindow)
		{
			Uint32 uiWindowID = SDL_GetWindowID(lpWindow);
			m_lpMouseOnWindow = getWindowByID(uiWindowID);
		}
		else
		{
			m_lpMouseOnWindow = nullptr;
		}
	}

	//{
	//	//활성화 되 있는 윈도우
	//	SDL_Window* lpWindow = SDL_GetMouse();
	//	if (lpWindow)
	//	{
	//		Uint32 uiWindowID = SDL_GetWindowID(lpWindow);
	//		m_lpMouseOnWindow = getWindowByID(uiWindowID);
	//	}
	//	else
	//	{
	//		m_lpFocusedWindow = nullptr;
	//	}
	//}
}

void cIECore::operateWindowEvent(const SDL_Event* _lpEvent)
{
	//이벤트에 해당하는 창 가져오고 없으면 반환
	cWindow* lpWindow = getWindowByID(_lpEvent->window.windowID);
	if (!lpWindow)
		return;

	switch (_lpEvent->window.event)
	{
	case SDL_WindowEventID::SDL_WINDOWEVENT_RESIZED:
	{
		lpWindow->resizeRenderer();
	}
	break;
	case SDL_WindowEventID::SDL_WINDOWEVENT_CLOSE:
	{
		lpWindow->callXButton();
	}
	break;
	}
}

void cIECore::update()
{

}

void cIECore::draw()
{
	std::map<std::string, cWindow*>::iterator ite = m_mapWindow.begin();
	//그릴게 없다
	if (ite == m_mapWindow.end())
		return;

	//드로우 한 기록 초기화
	m_iDrawCompleteCounter.store(0);
	for (; ite != m_mapWindow.end(); ++ite)
		ite->second->resetDrawed();

	//각 창 스레드에 그리라고 전달
	m_cvDrawThreadWaiter.notify_all();

	std::mutex mtxWaiter;
	std::unique_lock<std::mutex> lkWaiter(mtxWaiter);

	//그리기 완료 때 까지 대기 / 엔진이 멈추면 얘도 멈춰줘야된다
	m_cvDrawCompleteWaiter.wait(lkWaiter, [&] {
		return isDrawComplete() || !isRunning();
		});
}

bool cIECore::beginEngine()
{
	if (m_pMainThread)
		return false;

	m_bIsRunning = true;

	m_pMainThread = new std::thread([&]() { mainThread(); });

	std::map<std::string, cWindow*>::iterator ite = m_mapWindow.begin();
	//스레드를 각자 파서 각자 그리도록
	for (; ite != m_mapWindow.end(); ++ite)
		ite->second->beginDrawThread();

	return true;
}

bool cIECore::endEngine()
{
	if (m_pMainThread != nullptr)
	{
		m_pMainThread->join();
		delete m_pMainThread;
		m_pMainThread = nullptr;
	}

	for (auto& [_, window] : m_mapWindow)//각 창 삭제
		window->close();

	SDL_Quit();
	return true;
}

bool cIECore::operateTextEdit(SDL_Event* _lpEvent)
{
	if (m_lpFocusedTextBox == nullptr
		|| !m_bTextEdit)
		return false;

	switch (_lpEvent->type)
	{
	case SDL_EventType::SDL_KEYDOWN:
	{
		if (!m_bUseIME)
		{
			//텍스트 지우기
			if (_lpEvent->key.keysym.sym == SDLK_BACKSPACE
				&& m_lpFocusedTextBox->getTextLength() > 0)
				m_lpFocusedTextBox->removeByBackspace();

			if (_lpEvent->key.keysym.sym == SDLK_DELETE
				&& m_lpFocusedTextBox->getTextLength() > 0)
				m_lpFocusedTextBox->removeByDelete();

			//커서 좌우로 이동
			if (_lpEvent->key.keysym.sym == SDLK_RIGHT)
				m_lpFocusedTextBox->cusorMoveNext();
			if (_lpEvent->key.keysym.sym == SDLK_LEFT)
				m_lpFocusedTextBox->cusorMovePrevious();

			//엔터
			if (_lpEvent->key.keysym.sym == SDLK_RETURN
				|| _lpEvent->key.keysym.sym == SDLK_KP_ENTER)
				m_lpFocusedTextBox->insertCusorPos("\n");
		}
	}
	break;
	case SDL_EventType::SDL_TEXTEDITING:
	{//조합형 입력중
		if (strlen(_lpEvent->text.text) == 0)
		{
			if (m_bUseIME)
				m_lpFocusedTextBox->removeIMEInput();
			m_bUseIME = false;
			break;
		}

		if (m_bUseIME)
			m_lpFocusedTextBox->removeIMEInput();

		m_lpFocusedTextBox->insertCusorPos(_lpEvent->text.text);
		m_lpFocusedTextBox->setIMELength(strlen(_lpEvent->text.text));
		m_bUseIME = true;
	}
	break;
	case SDL_EventType::SDL_TEXTINPUT:
	{//입력
		if (m_bUseIME)
			m_lpFocusedTextBox->removeIMEInput();

		m_bUseIME = false;
		m_lpFocusedTextBox->insertCusorPos(_lpEvent->text.text);
	}
	break;
	default:
		return false;
	break;
	}

	return true;
}

std::optional<SDL_Rect> cIECore::getTextEditPosition()
{
	std::lock_guard lockGuard(m_mtxTextEdit);
	if (!m_bTextEdit)
		return std::nullopt;

	return m_rtTextEditPosition;
}