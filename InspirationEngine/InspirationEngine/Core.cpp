#include "InspirationEngine.h"

void InspirationEngine::mainThread()
{
	while(m_bIsRunning)
	{
		std::chrono::system_clock::time_point StartTime = std::chrono::system_clock::now();

		operateEvent();

		update();

		draw();

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
	//큐에서 이벤트 가져옴
	std::deque<SDL_Event> dqEventQueue;
	m_mtxEvent.lock();
	std::swap(dqEventQueue, m_dqEventQueue);
	m_mtxEvent.unlock();


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
		}
		break;
		case SDL_EventType::SDL_WINDOWEVENT:
		{
			operateWindowEvent(&Event);
		}
		break;
		}
	}
}

void InspirationEngine::operateWindowEvent(const SDL_Event* _lpEvent)
{
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

	//스레드를 각자 파서 각자 그리도록
	for(; ite != m_mapWindow.end(); ++ite)
		ite->second->beginDrawThread();
	
	//업데이트는 모든 그리기가 끝나야 갈 수 있다
	for(ite = m_mapWindow.begin(); ite != m_mapWindow.end(); ++ite)
	{
		ite->second->joinDrawThread();
		ite->second->deleteThread();
	}
}

bool InspirationEngine::beginEngine()
{
	if(m_pMainThread)
		return false;

	m_bIsRunning = true;

	m_pMainThread = new std::thread([&]() {mainThread(); });

	return true;
}
