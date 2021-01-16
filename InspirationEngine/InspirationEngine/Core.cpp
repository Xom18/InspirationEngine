#include <future>
#include "InspirationEngine.h"

void InspirationEngine::mainThread()
{
	while(m_bIsRunning)
	{
		std::chrono::system_clock::time_point StartTime = std::chrono::system_clock::now();

		m_iOperatePahse = eENGINE_PAHSE_OPERATE_EVENT;
		operateEvent();

		m_iOperatePahse = eENGINE_PAHSE_UPDATE;
		update();

		m_iOperatePahse = eENGINE_PAHSE_DRAW;
		draw();

		m_iOperatePahse = eENGINE_PAHSE_COMPLETE;
		//남은 시간만큼 재운다
		std::chrono::duration<double> EndTime = std::chrono::system_clock::now() - StartTime;
		std::chrono::milliseconds msEndTime = std::chrono::duration_cast<std::chrono::milliseconds>(EndTime);

		//처리가 너무 오래걸렸다
		if(m_iTickRate < msEndTime.count())
			continue;

		std::chrono::milliseconds msSleepTime(m_iTickRate - msEndTime.count());
		std::this_thread::sleep_for(msSleepTime);
	}

	return;
}

void InspirationEngine::operateEvent()
{
	//마우스 위치 갱신
	{
		int iMouseX = 0;
		int iMouseY = 0;
		SDL_GetMouseState(&iMouseX, &iMouseY);
		m_Input.updateMousePos(iMouseX, iMouseY);

		//마우스가 올라와있는 윈도우
		SDL_Window* lpWindow = SDL_GetMouseFocus();
		if(lpWindow)
		{
			Uint32 uiWindowID = SDL_GetWindowID(lpWindow);
			m_lpMouseOnWindow = getWindowByID(uiWindowID);
		}
		else
		{
			m_lpMouseOnWindow = nullptr;
		}
	}

	{
		//활성화 되 있는 윈도우
		SDL_Window* lpWindow = SDL_GetMouseFocus();
		if(lpWindow)
		{
			Uint32 uiWindowID = SDL_GetWindowID(lpWindow);
			m_lpMouseOnWindow = getWindowByID(uiWindowID);
		}
		else
		{
			m_lpFocusedWindow = nullptr;
		}
	}

	//큐에서 이벤트 가져옴
	std::deque<SDL_Event> dqEventQueue;
	m_mtxEvent.lock();
	std::swap(dqEventQueue, m_dqEventQueue);
	m_mtxEvent.unlock();

	SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
	SDL_SetCursor(cursor);

	//이벤트 처리
	while(!dqEventQueue.empty())
	{
		SDL_Event Event = dqEventQueue.front();
		dqEventQueue.pop_front();
		switch(Event.type)
		{
		case SDL_EventType::SDL_KEYDOWN:
		case SDL_EventType::SDL_KEYUP:
		{
			m_Input.setKeyState(Event.key.keysym.scancode, Event.key.state);

			if(Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_BACKSPACE)
			{
				if(strInputtingText.length())
				{
					const char* lpText = strInputtingText.c_str();
					int iLength = static_cast<int>(strInputtingText.length());

					if((lpText[iLength - 1] & 0b10000000) == 0)
					{//아스키쪽인지 확인
						strInputtingText.pop_back();
					}
					else
					{//UTF-8이다
						//몇개 지워야되는지 체크
						int iDeleteCount = 0;
						for(int i = iLength - 1; i >= 0; --i)
						{
							++iDeleteCount;
							if((lpText[i] & 0b11000000) == 0b11000000)
								break;
						}

						//지워야되는 만큼 삭제
						strInputtingText.erase(static_cast<size_t>(iLength) - iDeleteCount);
					}
				}
			}
		}
		break;
		case SDL_EventType::SDL_WINDOWEVENT:
		{
			operateWindowEvent(&Event);
		}
		break;
		case SDL_EventType::SDL_TEXTEDITING:
		{//조합형 입력중
//			strInputtingText.clear();
//			strInputtingText += Event.text.text;
		}
		break;
		case SDL_EventType::SDL_TEXTINPUT:
		{//입력
			strInputtingText += Event.text.text;
		}
		break;
		}
	}
}

void InspirationEngine::operateWindowEvent(const SDL_Event* _lpEvent)
{
	//이벤트에 해당하는 창 가져오고 없으면 반환
	cWindow* lpWindow = getWindowByID(_lpEvent->window.windowID);
	if(!lpWindow)
		return;

	switch(_lpEvent->window.event)
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

void InspirationEngine::update()
{

}

void InspirationEngine::draw()
{
	std::map<std::string, cWindow*>::iterator ite = m_mapWindow.begin();
	//그릴게 없다
	if(ite == m_mapWindow.end())
		return;

	//드로우 한 기록 초기화
	m_iDrawCompleteCounter.store(0);
	for(; ite != m_mapWindow.end(); ++ite)
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

bool InspirationEngine::beginEngine()
{
	if(m_pMainThread)
		return false;

	m_bIsRunning = true;

	m_pMainThread = new std::thread([&]() {mainThread(); });

	std::map<std::string, cWindow*>::iterator ite = m_mapWindow.begin();
	//스레드를 각자 파서 각자 그리도록
	for(; ite != m_mapWindow.end(); ++ite)
		ite->second->beginDrawThread();

	return true;
}
