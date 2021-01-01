#pragma once

class InspirationEngine
{
public:
	cInput		m_Input;		//입력, 클릭이나 창 내부 처리도 여기서 받은다음 각 창으로 보냄
	cDebugInfo	m_DebugInfo;	//디버그 툴
	cWindow* m_lpMainWindow = nullptr;		//메인 윈도우
	cWindow* m_lpFocusedWindow = nullptr;	//포커스 가있는 윈도우

private:
	std::mutex m_mtxEvent;
	std::map<std::string, cWindow*>	m_mapWindow;//윈도우
	std::map<Uint32, cWindow*>	m_mapWindowByID;//윈도우
	std::thread* m_pMainThread = nullptr;		//메인 스레드
	int m_iTickRate = 16;						//메인스레드 처리간격(ms)
	bool m_bIsRunning = false;					//구동중 여부
	std::deque<SDL_Event> m_dqEventQueue;		//처리 안한 이벤트 큐

public:

	/// <summary>
	/// 생성자
	/// </summary>
	InspirationEngine(){}

	/// <summary>
	///	소멸자
	/// </summary>
	~InspirationEngine()
	{
		std::map<std::string, cWindow*>::iterator ite = m_mapWindow.begin();
		for(; ite != m_mapWindow.end(); ++ite)
			delete ite->second;
		m_mapWindow.clear();
		m_lpMainWindow = nullptr;
	}

	/// <summary>
	/// 엔진 시작
	/// </summary>
	/// <returns></returns>
	bool beginEngine();	

	/// <summary>
	/// 엔진 중단
	/// </summary>
	void stopEngine()
	{
		m_bIsRunning = false;
	}

	/// <summary>
	/// 구동중인지
	/// </summary>
	/// <returns>true / false</returns>
	bool isRunning()
	{
		return m_bIsRunning;
	}

	/// <summary>
	/// 처리 간격 설정
	/// </summary>
	/// <param name="_iTick">처리 간격(ms)</param>
	void setTickRate(int _iTick)
	{
		m_iTickRate = _iTick;
	}

	/// <summary>
	/// 처리 간격 받아오기
	/// </summary>
	/// <returns>처리 간격(ms)</returns>
	int getTickRate()
	{
		return m_iTickRate;
	}

	/// <summary>
	/// 동적할당 된 window를 해당 엔진에 할당
	/// </summary>
	/// <param name="_csID">해당 창이 가질 ID</param>
	/// <param name="_pWindow">동적할당 된 cWindow 변수</param>
	/// <returns>추가 성공했으면 true / 이미 같은 ID로 할당 된 window가 존재하면 false</returns>
	bool addNewWindow(const char* _csID, cWindow* _pWindow)
	{
		//동일한 ID의 창이 있으면 안되기때문에 처리
		if(getWindow(_csID) != nullptr)
			return false;

		if(m_mapWindow.empty())
			m_lpMainWindow = _pWindow;
		m_mapWindow.insert(std::pair<std::string, cWindow*>(_csID, _pWindow));
		return true;
	}

	/// <summary>
	/// 창 받아오기
	/// </summary>
	/// <param name="_csID">받아 올 창의 ID</param>
	/// <returns></returns>
	cWindow* getWindow(const char* _csID)
	{
		std::map<std::string, cWindow*>::iterator ite = m_mapWindow.find(_csID);
		if(ite == m_mapWindow.end())
			return nullptr;
		return ite->second;
	}

	/// <summary>
	/// 메인 창 설정
	/// </summary>
	void setMainWindow(cWindow* _pWindow)
	{
		m_lpMainWindow = _pWindow;
	}
	/// <summary>
	/// 메인 창 받아오기
	/// </summary>
	/// <returns>ID가 Main인 창 받아옴 / 없으면 nullptr</returns>
	cWindow* getMainWindow()
	{
		return m_lpMainWindow;
	}

	/// <summary>
	/// 해당 ID의 창 삭제
	/// </summary>
	/// <param name="_csID">삭제 할 창의 ID</param>
	void destroyWindow(const char* _csID)
	{
		std::map<std::string, cWindow*>::iterator ite = m_mapWindow.find(_csID);
		if(ite == m_mapWindow.end())
			return;

		if(ite->second == m_lpMainWindow)
			m_lpMainWindow = nullptr;

		delete ite->second;
		m_mapWindow.erase(ite);
	}

	/// <summary>
	/// 이벤트 처리 큐에 넣기
	/// </summary>
	/// <param name="_lpEvent">이벤트</param>
	void eventPushBack(SDL_Event* _lpEvent)
	{
		m_mtxEvent.lock();
		m_dqEventQueue.push_back(*_lpEvent);

		m_mtxEvent.unlock();
	}

	/// <summary>
	/// SDL Window Id로 찾을 수 있게 윈도우 등록
	/// </summary>
	/// <param name="_lpWindow">등록 할 윈도우</param>
	void addWindowIndex(cWindow* _lpWindow)
	{
		SDL_Window* lpWindow = _lpWindow->getSDLWindow();
		Uint32 uiID = SDL_GetWindowID(lpWindow);
		m_mapWindowByID.insert(std::pair<Uint32, cWindow*>(uiID, _lpWindow));
	}

	/// <summary>
	/// SDL Window Id로 Window받아오기
	/// </summary>
	/// <param name="_iID">window ID</param>
	/// <returns>해당하는 ID의 창을 가진 window</returns>
	cWindow* getWindowByID(Uint32 _uiID)
	{
		std::map<Uint32, cWindow*>::iterator ite = m_mapWindowByID.find(_uiID);
		if(ite == m_mapWindowByID.end())
			return nullptr;
		return ite->second;
	}

private:
	/// <summary>
	/// 주 처리 스레드
	/// </summary>
	void mainThread();

	/// <summary>
	/// 이벤트 처리 함수
	/// </summary>
	void operateEvent();

	/// <summary>
	/// 윈도우 이벤트 처리용
	/// </summary>
	/// <param name="_lpEvent">이벤트</param>
	void operateWindowEvent(const SDL_Event* _lpEvent);

	/// <summary>
	/// 업데이트용
	/// </summary>
	void update();

	/// <summary>
	/// 그리는용
	/// </summary>
	void draw();

};