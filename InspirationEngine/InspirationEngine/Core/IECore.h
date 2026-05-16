#pragma once

enum class EnginePhase : int32_t
{
	NaN = 0,
	OperateEvent,
	Update,
	Draw,
	Complete,
};

class IECore
{
public:
	IECore() {}

	~IECore()
	{
		m_windows.clear();
		m_windowsByID.clear();
		m_mainWindow = nullptr;

		if (m_mainThread != nullptr)
			m_mainThread->join();
	}

	static IEInput&         GetInput()     { return m_Input; }
	static IEDebugInfo&     GetDebugInfo() { return m_DebugInfo; }
	static IEFontManager&   GetFont()      { return m_Font; }
	static IESpriteManager& GetSprite()    { return m_Sprite; }
	static IEAtlasManager&  GetAtlas()     { return m_Atlas; }
	static IESceneManager&  GetScene()     { return m_Scene; }

	static IEWindow*  GetMouseOnWindow()  { return m_mouseOnWindow; }
	static IEWindow*  GetFocusedWindow()  { return m_focusedWindow; }
	static IETextBox* GetFocusedTextBox() { return m_focusedTextBox; }
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
	static bool BeginEngine();

	/// <summary>
	/// 엔진 종료
	/// </summary>
	static bool EndEngine();

	/// <summary>
	/// 엔진 중단
	/// </summary>
	static void StopEngine()
	{
		m_isRunning = false;
		m_drawThreadWaiter.notify_all();
		m_drawCompleteWaiter.notify_all();
	}

	/// <summary>
	/// 구동중인지
	/// </summary>
	static bool IsRunning()
	{
		return m_isRunning;
	}

	/// <summary>
	/// 직전 프레임 경과 시간 (초) 반환
	/// </summary>
	static float GetDeltaTime()
	{
		return m_deltaTime;
	}

	/// <summary>
	/// 직전 프레임 경과 시간 (ms) 반환
	/// </summary>
	static uint64_t GetDeltaTimeMs()
	{
		return m_deltaTimeMs;
	}

	/// <summary>
	/// 처리 간격 설정
	/// </summary>
	/// <param name="tick">틱 간격 (ms)</param>
	static void SetTickRate(int32_t tick)
	{
		m_tickRate = tick;
	}

	/// <summary>
	/// 처리 간격 받아오기
	/// </summary>
	static int32_t GetTickRate()
	{
		return m_tickRate;
	}

	/// <summary>
	/// 동적할당 된 window를 엔진에 등록
	/// </summary>
	/// <param name="id">창 식별 ID</param>
	/// <param name="window">등록할 창 (소유권 이전)</param>
	static bool AddNewWindow(const char* id, IEWindow* window)
	{
		if (GetWindow(id) != nullptr)
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
	static IEWindow* GetWindow(const char* id)
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
	static void SetMainWindow(IEWindow* window)
	{
		m_mainWindow = window;
	}

	/// <summary>
	/// 메인 창 받아오기
	/// </summary>
	static IEWindow* GetMainWindow()
	{
		return m_mainWindow;
	}

	/// <summary>
	/// 해당 ID의 창 삭제
	/// </summary>
	/// <param name="id">삭제할 창 ID</param>
	static void DestroyWindow(const char* id)
	{
		auto ite = m_windows.find(id);
		if (ite == m_windows.end())
			return;

		if (ite->second.get() == m_mainWindow)
			m_mainWindow = nullptr;

		m_windows.erase(ite);
	}

	/// <summary>
	/// 엔진 루프 안전 지점(다음 틱 시작)에서 창을 등록하고 드로우 스레드를 시작.
	/// 엔진 구동 중 동적 창 생성 시 사용.
	/// </summary>
	/// <param name="id">창 식별 ID</param>
	/// <param name="window">등록할 창 (소유권 이전)</param>
	static void RequestAddWindow(const char* id, IEWindow* window);

	/// <summary>
	/// 엔진 루프 안전 지점(다음 틱 시작)에서 창을 드로우 스레드 종료 후 제거.
	/// 엔진 구동 중 동적 창 제거 시 사용.
	/// </summary>
	/// <param name="id">제거할 창 ID</param>
	static void RequestRemoveWindow(const char* id);

	/// <summary>
	/// 이벤트 처리 큐에 넣기
	/// </summary>
	/// <param name="event">추가할 SDL 이벤트</param>
	static void EventPushBack(SDL_Event* event)
	{
		m_eventMutex.lock();
		m_eventQueue.push_back(*event);
		m_eventMutex.unlock();
	}

	/// <summary>
	/// IME 입력 중이면 커서 위치 반환, 아니면 nullopt
	/// </summary>
	static std::optional<SDL_Rect> GetTextEditPosition();

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
	static void UpdateTextEditPosition(SDL_Rect& rect)
	{
		m_textEditMutex.lock();
		m_textEditPosition = rect;
		m_textEditMutex.unlock();
	}

	/// <summary>
	/// SDL Window ID로 찾을 수 있게 윈도우 등록
	/// </summary>
	/// <param name="window">등록할 창</param>
	static void AddWindowIndex(IEWindow* window)
	{
		SDL_Window* sdlWindow = window->GetSDLWindow();
		Uint32 id = SDL_GetWindowID(sdlWindow);
		m_windowsByID.insert(std::pair<Uint32, IEWindow*>(id, window));
	}

	/// <summary>
	/// SDL Window ID로 Window 받아오기
	/// </summary>
	/// <param name="id">SDL 윈도우 ID</param>
	static IEWindow* GetWindowByID(Uint32 id)
	{
		auto ite = m_windowsByID.find(id);
		if (ite == m_windowsByID.end())
			return nullptr;
		return ite->second;
	}

	/// <summary>
	/// 화면 그리는 스레드 대기용 변수 가져오기
	/// </summary>
	static std::condition_variable* GetDrawWaiter()
	{
		return &m_drawThreadWaiter;
	}

	/// <summary>
	/// 창 그릴 때마다 1씩 증가, 전부 완료되면 draw 완료 신호
	/// </summary>
	static void IncreaseDrawCompleteCount()
	{
		m_drawCompleteCounter.fetch_add(1);
		if (m_drawCompleteCounter.load() >= static_cast<int32_t>(m_windows.size()))
			m_drawCompleteWaiter.notify_all();
	}

	/// <summary>
	/// 그리는게 완료되었는지
	/// </summary>
	static bool IsDrawComplete()
	{
		int32_t value = m_drawCompleteCounter.load();
		return value >= static_cast<int32_t>(m_windows.size());
	}

	/// <summary>
	/// 엔진의 현재 상태 가져오기
	/// </summary>
	static EnginePhase GetEnginePhase()
	{
		return m_operatePhase;
	}

	/// <summary>
	/// 플랫폼별 IME composition 폴링 (SDL3가 TEXT_EDITING 미지원 플랫폼용).
	/// 메인 스레드에서 매 프레임 호출.
	/// </summary>
	/// <param name="focusWin">현재 키보드 포커스 창 (nullptr 가능)</param>
	static void PollPlatformIME(SDL_Window* focusWin);

	/// <summary>
	/// 메인 스레드에서 실행할 태스크를 큐에 추가.
	/// SDL 창 생성 등 HWND 스레드 어피니티가 필요한 작업에 사용.
	/// </summary>
	/// <param name="task">실행할 함수 객체</param>
	static void PostMainThreadTask(std::function<void()> task);

	/// <summary>
	/// 큐에 쌓인 메인 스레드 태스크를 모두 실행. 메인 스레드에서 호출.
	/// </summary>
	static void RunMainThreadTasks();

	/// <summary>
	/// 텍스트 편집 이벤트 처리 — 처리했으면 true 반환
	/// </summary>
	/// <param name="event">처리할 SDL 이벤트</param>
	static bool OperateTextEdit(SDL_Event* event);

private:
	static void MainThread();
	static void OperateEvent();
	static void Update(float deltaTime);
	static void Draw();
	static void ProcessPendingWindows();

private:
	static IEInput          m_Input;
	static IEDebugInfo      m_DebugInfo;
	static IEFontManager    m_Font;
	static IESpriteManager  m_Sprite;
	static IEAtlasManager   m_Atlas;
	static IESceneManager   m_Scene;
	static float            m_deltaTime;
	static uint64_t         m_deltaTimeMs;
	static IEWindow*        m_mainWindow;
	static IEWindow*        m_mouseOnWindow;
	static IEWindow*        m_focusedWindow;
	static IETextBox*       m_focusedTextBox;

	static EnginePhase      m_operatePhase;
	static std::mutex       m_eventMutex;
	static std::map<std::string, std::unique_ptr<IEWindow>> m_windows;
	static std::map<Uint32, IEWindow*>                      m_windowsByID;
	static std::unique_ptr<std::thread>                     m_mainThread;
	static int32_t          m_tickRate;
	static bool             m_isRunning;
	static std::deque<SDL_Event> m_eventQueue;

	static std::atomic<int32_t>    m_drawCompleteCounter;
	static std::condition_variable m_drawThreadWaiter;
	static std::condition_variable m_drawCompleteWaiter;

	static bool             m_useIME;
	static std::mutex       m_textEditMutex;
	static bool             m_textEdit;
	static SDL_Rect         m_textEditPosition;

	static std::string      m_pendingIMEComposition;
	static bool             m_imeCompositionDirty;

	static std::mutex                                           m_pendingWindowsMutex;
	static std::vector<std::pair<std::string, IEWindow*>>       m_pendingWindowsToAdd;
	static std::vector<std::string>                             m_pendingWindowsToRemove;

	static std::mutex                                           m_mainTasksMutex;
	static std::vector<std::function<void()>>                   m_mainTasks;
};
