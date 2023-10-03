#pragma once

enum
{
	eENGINE_PAHSE_NaN = 0,		//구동중 아님
	eENGINE_PAHSE_OPERATE_EVENT,//업데이트
	eENGINE_PAHSE_UPDATE,		//업데이트
	eENGINE_PAHSE_DRAW,			//그리기
	eENGINE_PAHSE_COMPLETE,		//처리완료(다음 틱 대기중)
};

//얘는 스태틱이다
class cIECore
{
public:
	static cInput		m_Input;			//입력, 클릭이나 창 내부 처리도 여기서 받은다음 각 창으로 보냄
	static cDebugInfo	m_DebugInfo;		//디버그 툴
	static cFontManager	m_Font;				//폰트 관리하는곳
	static cWindow*		m_lpMainWindow;		//메인 윈도우
	static cWindow*		m_lpMouseOnWindow;	//마우스가 올라가 있는 윈도우
	static cWindow*		m_lpFocusedWindow;	//선택 되있는 윈도우
	static cTextBox*	m_lpFocusedTextBox;	//선택 되 있는 텍스트 박스
private:
	
	static int m_iOperatePahse;		//현재 어느걸 처리중인지
	static std::mutex m_mtxEvent;	//이벤트 뮤텍스
	static std::map<std::string, cWindow*>	m_mapWindow;//윈도우
	static std::map<Uint32, cWindow*>	m_mapWindowByID;//윈도우
	static std::thread* m_pMainThread;			//메인 스레드
	static int m_iTickRate;						//메인스레드 처리간격(ms)
	static bool m_bIsRunning;					//구동중 여부
	static std::deque<SDL_Event> m_dqEventQueue;//처리 안한 이벤트 큐

	static std::atomic<int>	m_iDrawCompleteCounter;	//그리기 완료 카운터
	static std::condition_variable m_cvDrawThreadWaiter;//각 창의 drawthread를 대기 시켜주는곳
	static std::condition_variable m_cvDrawCompleteWaiter;//각 창의 drawthread를 대기 시켜주는곳

public:

	/// <summary>
	/// 생성자
	/// </summary>
	cIECore()
	{
	}

	/// <summary>
	///	소멸자
	/// </summary>
	~cIECore()
	{
		std::map<std::string, cWindow*>::iterator ite = m_mapWindow.begin();
		for(; ite != m_mapWindow.end(); ++ite)
			delete ite->second;
		m_mapWindow.clear();
		m_mapWindowByID.clear();
		m_lpMainWindow = nullptr;

		if(m_pMainThread)
		{
			m_pMainThread->join();
			delete m_pMainThread;
		}
	}

	/// <summary>
	/// 엔진 시작
	/// </summary>
	/// <returns></returns>
	static bool beginEngine();

	/// <summary>
	/// 엔진 종료
	/// </summary>
	static bool endEngine();

	/// <summary>
	/// 엔진 중단
	/// </summary>
	static void stopEngine()
	{
		m_bIsRunning = false;
		m_cvDrawThreadWaiter.notify_all();	//각 창 스레드 중단
		m_cvDrawCompleteWaiter.notify_all();//그리는거 기다리는거 중단
	}

	/// <summary>
	/// 구동중인지
	/// </summary>
	/// <returns>true / false</returns>
	static bool isRunning()
	{
		return m_bIsRunning;
	}

	/// <summary>
	/// 처리 간격 설정
	/// </summary>
	/// <param name="_iTick">처리 간격(ms)</param>
	static void setTickRate(int _iTick)
	{
		m_iTickRate = _iTick;
	}

	/// <summary>
	/// 처리 간격 받아오기
	/// </summary>
	/// <returns>처리 간격(ms)</returns>
	static int getTickRate()
	{
		return m_iTickRate;
	}

	/// <summary>
	/// 동적할당 된 window를 해당 엔진에 할당
	/// </summary>
	/// <param name="_csID">해당 창이 가질 ID</param>
	/// <param name="_pWindow">동적할당 된 cWindow 변수</param>
	/// <returns>추가 성공했으면 true / 이미 같은 ID로 할당 된 window가 존재하면 false</returns>
	static bool addNewWindow(const char* _csID, cWindow* _pWindow)
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
	static cWindow* getWindow(const char* _csID)
	{
		std::map<std::string, cWindow*>::iterator ite = m_mapWindow.find(_csID);
		if(ite == m_mapWindow.end())
			return nullptr;
		return ite->second;
	}

	/// <summary>
	/// 메인 창 설정
	/// </summary>
	static void setMainWindow(cWindow* _pWindow)
	{
		m_lpMainWindow = _pWindow;
	}
	/// <summary>
	/// 메인 창 받아오기
	/// </summary>
	/// <returns>ID가 Main인 창 받아옴 / 없으면 nullptr</returns>
	static cWindow* getMainWindow()
	{
		return m_lpMainWindow;
	}

	/// <summary>
	/// 해당 ID의 창 삭제
	/// </summary>
	/// <param name="_csID">삭제 할 창의 ID</param>
	static void destroyWindow(const char* _csID)
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
	static void eventPushBack(SDL_Event* _lpEvent)
	{
		m_mtxEvent.lock();
		m_dqEventQueue.push_back(*_lpEvent);

		m_mtxEvent.unlock();
	}

	/// <summary>
	/// SDL Window Id로 찾을 수 있게 윈도우 등록
	/// </summary>
	/// <param name="_lpWindow">등록 할 윈도우</param>
	static void addWindowIndex(cWindow* _lpWindow)
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
	static cWindow* getWindowByID(Uint32 _uiID)
	{
		std::map<Uint32, cWindow*>::iterator ite = m_mapWindowByID.find(_uiID);
		if(ite == m_mapWindowByID.end())
			return nullptr;
		return ite->second;
	}

	/// <summary>
	/// 화면 그리는 스레드 대기용 변수 가져오기
	/// </summary>
	static std::condition_variable* getDrawWaiter()		//화면 그리기 대기용
	{
		return &m_cvDrawThreadWaiter;
	}
	
	/// <summary>
	/// 창 그릴 때 마다 1씩 증가시켜서 다 되면 draw가 끝났다고 알려주는 함수
	/// </summary>
	static void increaseDrawCompleteCount()
	{
		m_iDrawCompleteCounter.fetch_add(1);
		if (m_iDrawCompleteCounter.load() >= static_cast<int>(m_mapWindow.size()))
			m_cvDrawCompleteWaiter.notify_all();
	}

	/// <summary>
	/// 그리는게 완료되었는지
	/// </summary>
	/// <returns></returns>
	static bool isDrawComplete()
	{
		int iValue = m_iDrawCompleteCounter.load();
		return iValue >= static_cast<int>(m_mapWindow.size());
	}

	/// <summary>
	/// 엔진의 현재 상태 가져오는거
	/// </summary>
	/// <returns>엔진의 현재 상태</returns>
	static int getEnginePhase()
	{
		return m_iOperatePahse;
	}

	static void operateTextEdit(SDL_Event* _lpEvent);

private:
	/// <summary>
	/// 주 처리 스레드
	/// </summary>
	static void mainThread();

	/// <summary>
	/// 이벤트 처리 함수
	/// </summary>
	static void operateEvent();

	/// <summary>
	/// 윈도우 이벤트 처리용
	/// </summary>
	/// <param name="_lpEvent">이벤트</param>
	static void operateWindowEvent(const SDL_Event* _lpEvent);

	/// <summary>
	/// 업데이트용
	/// </summary>
	static void update();

	/// <summary>
	/// 그리는용
	/// </summary>
	static void draw();

};