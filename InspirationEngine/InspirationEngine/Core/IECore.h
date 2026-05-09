#pragma once

enum class EnginePhase : int32_t
{
	NaN = 0,			//구동중 아님
	OperateEvent,		//이벤트 처리
	Update,				//업데이트
	Draw,				//그리기
	Complete,			//처리완료(다음 틱 대기중)
};

//얘는 스태틱이다
class IECore
{
private:
	static IEInput          m_Input;
	static IEDebugInfo        m_DebugInfo;
	static IEFontManager    m_Font;
	static IESpriteManager  m_Sprite;
	static IEAtlasManager   m_Atlas;
	static IESceneManager   m_Scene;
	static float            m_deltaTime;
	static uint64_t         m_deltaTimeMs;
	static IEWindow*        m_mainWindow;
	static IEWindow*        m_mouseOnWindow;
	static IEWindow*        m_focusedWindow;
	static IETextBox*         m_focusedTextBox;

	static EnginePhase		m_operatePhase;		//현재 어느걸 처리중인지
	static std::mutex m_eventMutex;			//이벤트 뮤텍스
	static std::map<std::string, std::unique_ptr<IEWindow>>	m_windows;		//윈도우
	static std::map<Uint32, IEWindow*>	m_windowsByID;		//윈도우
	static std::unique_ptr<std::thread> m_mainThread;		//메인 스레드
	static int32_t m_tickRate;						//메인스레드 처리간격(ms)
	static bool m_isRunning;					//구동중 여부
	static std::deque<SDL_Event> m_eventQueue;	//처리 안한 이벤트 큐

	static std::atomic<int32_t>	m_drawCompleteCounter;	//그리기 완료 카운터
	static std::condition_variable m_drawThreadWaiter;	//각 창의 drawthread를 대기 시켜주는곳
	static std::condition_variable m_drawCompleteWaiter;//각 창의 drawthread를 대기 시켜주는곳

	static bool m_useIME;

	static std::mutex m_textEditMutex;			// IME 사용 처리를 위한 뮤텍스
	static bool m_textEdit;						// IME 사용중인지
	static SDL_Rect m_textEditPosition;			// IME 사용중일때 IME의 위치

	static std::string m_pendingIMEComposition;	// Windows IME 폴링 결과 (메인스레드 → 엔진스레드)
	static bool m_imeCompositionDirty;			// 새 composition 대기중

public:

	IECore()
	{
	}

	~IECore()
	{
		m_windows.clear();
		m_windowsByID.clear();
		m_mainWindow = nullptr;

		if (m_mainThread)
			m_mainThread->join();
	}

	static IEInput&          GetInput()     { return m_Input; }
	static IEDebugInfo&        GetDebugInfo() { return m_DebugInfo; }
	static IEFontManager&    GetFont()      { return m_Font; }
	static IESpriteManager&  GetSprite()    { return m_Sprite; }
	static IEAtlasManager&   GetAtlas()     { return m_Atlas; }
	static IESceneManager&   GetScene()     { return m_Scene; }

	static IEWindow*  GetMouseOnWindow()  { return m_mouseOnWindow; }
	static IEWindow*  GetFocusedWindow()  { return m_focusedWindow; }
	static IETextBox*   GetFocusedTextBox() { return m_focusedTextBox; }
	static void SetMouseOnWindow(IEWindow* window)  { m_mouseOnWindow = window; }
	static void SetFocusedWindow(IEWindow* window)  { m_focusedWindow = window; }
	static void SetFocusedTextBox(IETextBox* textBox)
	{
		m_focusedTextBox = textBox;
		if (textBox != nullptr)
			m_textEdit = true;
	}

	/// <summary>
	/// 엔진 시작
	/// </summary>
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
		m_isRunning = false;
		m_drawThreadWaiter.notify_all();	//각 창 스레드 중단
		m_drawCompleteWaiter.notify_all();//그리는거 기다리는거 중단
	}

	/// <summary>
	/// 구동중인지
	/// </summary>
	static bool isRunning()
	{
		return m_isRunning;
	}

	/// <summary>
	/// 직전 프레임 경과 시간 (초) 반환
	/// </summary>
	static float getDeltaTime()
	{
		return m_deltaTime;
	}

	/// <summary>
	/// 직전 프레임 경과 시간 (ms) 반환
	/// </summary>
	static uint64_t getDeltaTimeMs()
	{
		return m_deltaTimeMs;
	}

	/// <summary>
	/// 처리 간격 설정
	/// </summary>
	/// <param name="tick">틱 간격 (ms)</param>
	static void setTickRate(int32_t tick)
	{
		m_tickRate = tick;
	}

	/// <summary>
	/// 처리 간격 받아오기
	/// </summary>
	static int32_t getTickRate()
	{
		return m_tickRate;
	}

	/// <summary>
	/// 동적할당 된 window를 해당 엔진에 할당
	/// </summary>
	/// <param name="id">창 식별 ID</param>
	/// <param name="window">등록할 창 (소유권 이전)</param>
	static bool addNewWindow(const char* id, IEWindow* window)
	{
		//동일한 ID의 창이 있으면 안되기때문에 처리
		if (getWindow(id) != nullptr)
			return false;

		if (m_windows.empty())
			m_mainWindow = window;
		m_windows[id] = std::unique_ptr<IEWindow>(window);
		return true;
	}

	/// <summary>
	/// 창 받아오기
	/// </summary>
	/// <param name="id">창 식별 ID</param>
	static IEWindow* getWindow(const char* id)
	{
		auto ite = m_windows.find(id);
		if (ite == m_windows.end())
			return nullptr;
		return ite->second.get();
	}

	/// <summary>
	/// 메인 창 설정
	/// </summary>
	/// <param name="window">메인으로 지정할 창</param>
	static void setMainWindow(IEWindow* window)
	{
		m_mainWindow = window;
	}
	/// <summary>
	/// 메인 창 받아오기
	/// </summary>
	static IEWindow* getMainWindow()
	{
		return m_mainWindow;
	}

	/// <summary>
	/// 해당 ID의 창 삭제
	/// </summary>
	/// <param name="id">삭제할 창 ID</param>
	static void destroyWindow(const char* id)
	{
		auto ite = m_windows.find(id);
		if (ite == m_windows.end())
			return;

		if (ite->second.get() == m_mainWindow)
			m_mainWindow = nullptr;

		m_windows.erase(ite);
	}

	/// <summary>
	/// 이벤트 처리 큐에 넣기
	/// </summary>
	/// <param name="event">추가할 SDL 이벤트</param>
	static void eventPushBack(SDL_Event* event)
	{
		m_eventMutex.lock();
		m_eventQueue.push_back(*event);
		m_eventMutex.unlock();
	}

	/// <summary>
	/// IME 입력 중이면 커서 위치 반환, 아니면 nullopt
	/// </summary>
	static std::optional<SDL_Rect> getTextEditPosition();

	/// <summary>
	/// Windows IME 폴링 결과를 엔진스레드로 전달 (메인스레드에서 호출)
	/// </summary>
	/// <param name="comp">현재 composition 문자열 (확정 전). 빈 문자열 = 종료</param>
	static void SetIMEComposition(const std::string& comp)
	{
		std::lock_guard<std::mutex> lock(m_eventMutex);
		m_pendingIMEComposition = comp;
		m_imeCompositionDirty   = true;
	}

	/// <summary>
	/// 텍스트 편집 커서 위치 갱신
	/// </summary>
	/// <param name="rect">커서 위치 Rect</param>
	static void updateTextEditPosition(SDL_Rect& rect)
	{
		m_textEditMutex.lock();
		m_textEditPosition = rect;
		m_textEditMutex.unlock();
	}

	/// <summary>
	/// SDL Window ID로 찾을 수 있게 윈도우 등록
	/// </summary>
	/// <param name="window">등록할 창</param>
	static void addWindowIndex(IEWindow* window)
	{
		SDL_Window* lpWindow = window->getSDLWindow();
		Uint32 uiID = SDL_GetWindowID(lpWindow);
		m_windowsByID.insert(std::pair<Uint32, IEWindow*>(uiID, window));
	}

	/// <summary>
	/// SDL Window ID로 Window 받아오기
	/// </summary>
	/// <param name="id">SDL 윈도우 ID</param>
	static IEWindow* getWindowByID(Uint32 id)
	{
		std::map<Uint32, IEWindow*>::iterator ite = m_windowsByID.find(id);
		if (ite == m_windowsByID.end())
			return nullptr;
		return ite->second;
	}

	/// <summary>
	/// 화면 그리는 스레드 대기용 변수 가져오기
	/// </summary>
	static std::condition_variable* getDrawWaiter()
	{
		return &m_drawThreadWaiter;
	}

	/// <summary>
	/// 창 그릴 때 마다 1씩 증가시켜서 다 되면 draw가 끝났다고 알려주는 함수
	/// </summary>
	static void increaseDrawCompleteCount()
	{
		m_drawCompleteCounter.fetch_add(1);
		if (m_drawCompleteCounter.load() >= static_cast<int32_t>(m_windows.size()))
			m_drawCompleteWaiter.notify_all();
	}

	/// <summary>
	/// 그리는게 완료되었는지
	/// </summary>
	static bool isDrawComplete()
	{
		int32_t iValue = m_drawCompleteCounter.load();
		return iValue >= static_cast<int32_t>(m_windows.size());
	}

	/// <summary>
	/// 엔진의 현재 상태 가져오는거
	/// </summary>
	static EnginePhase getEnginePhase()
	{
		return m_operatePhase;
	}

	/// <summary>
	/// 플랫폼별 IME composition 폴링 (SDL3가 TEXT_EDITING 미지원 플랫폼용).
	/// 메인 스레드에서 매 프레임 호출.
	/// </summary>
	/// <param name="focusWin">현재 키보드 포커스 창 (nullptr 가능)</param>
	static void pollPlatformIME(SDL_Window* focusWin);

	/// <summary>
	/// 텍스트 편집 이벤트 처리 — 처리했으면 true 반환
	/// </summary>
	/// <param name="event">처리할 SDL 이벤트</param>
	static bool operateTextEdit(SDL_Event* event);

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
	/// 업데이트용
	/// </summary>
	static void update(float deltaTime);

	/// <summary>
	/// 그리는용
	/// </summary>
	static void draw();

};
