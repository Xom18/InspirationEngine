#pragma once

// Windows의 CreateWindow 매크로가 멤버 함수 선언을 망가뜨리는 것을 방지
#ifdef CreateWindow
#undef CreateWindow
#endif

class IECore;

class IEWindow
{
public:
	IEWindow();
	~IEWindow();

	/// <summary>
	/// 프레임 업데이트
	/// </summary>
	/// <param name="deltaTime">경과 시간 (초)</param>
	virtual void Update(float deltaTime) {}

	/// <summary>
	/// 그리기
	/// </summary>
	virtual void Draw() {}

	/// <summary>
	/// X버튼을 눌렀을 때
	/// </summary>
	virtual void CallXButton() {}

	/// <summary>
	/// 창 생성
	/// </summary>
	/// <param name="title">창 제목</param>
	/// <param name="width">너비 (px)</param>
	/// <param name="height">높이 (px)</param>
	/// <param name="x">창 위치 X (기본값: 화면 중앙)</param>
	/// <param name="y">창 위치 Y (기본값: 화면 중앙)</param>
	/// <param name="windowFlag">SDL 창 플래그</param>
	/// <param name="rendererCount">소프트웨어 렌더러 개수</param>
	bool CreateWindow(const char* title, int32_t width, int32_t height, int32_t x = SDL_WINDOWPOS_CENTERED, int32_t y = SDL_WINDOWPOS_CENTERED, SDL_WindowFlags windowFlag = 0, int32_t rendererCount = 1);

	/// <summary>
	/// 창이 닫혀있는지 여부
	/// </summary>
	bool Closed();

	/// <summary>
	/// 창 닫기
	/// </summary>
	void Close();

	/// <summary>
	/// 렌더러 크기를 현재 창에 맞게 재지정
	/// </summary>
	void ResizeRenderer();

	/// <summary>
	/// 갖고있는 변수 초기화 (창 닫힘)
	/// </summary>
	void Reset();

	/// <summary>
	/// 렌더러 포인터 받아오기
	/// </summary>
	/// <param name="rendererIndex">렌더러 인덱스</param>
	IERenderer* GetRenderer(int32_t rendererIndex)
	{
		if (static_cast<int32_t>(m_renderers.size()) <= rendererIndex)
			return nullptr;
		return &m_renderers[rendererIndex];
	}

	/// <summary>
	/// 창 포인터 받아오기
	/// </summary>
	SDL_Window* GetSDLWindow()
	{
		return m_sdlWindow;
	}

	/// <summary>
	/// 드로우 끝난 창 렌더
	/// </summary>
	void Render();

	/// <summary>
	/// 이 창이 하이드 상태인지
	/// </summary>
	bool IsWindowHide()
	{
		return SDL_GetWindowFlags(m_sdlWindow) & SDL_WINDOW_HIDDEN;
	}

	/// <summary>
	/// 창 표시
	/// </summary>
	void ShowWindow()
	{
		SDL_ShowWindow(m_sdlWindow);
	}

	/// <summary>
	/// 창 숨기기
	/// </summary>
	void HideWindow()
	{
		SDL_HideWindow(m_sdlWindow);
	}

	/// <summary>
	/// 드로우 스레드 시작
	/// </summary>
	void BeginDrawThread();

	/// <summary>
	/// 드로우 스레드 종료 대기
	/// </summary>
	void JoinDrawThread()
	{
		if (m_drawThread == nullptr)
			return;
		m_drawThread->join();
	}

	/// <summary>
	/// 드로우 스레드 해제
	/// </summary>
	void DeleteThread()
	{
		m_drawThread.reset();
	}

	/// <summary>
	/// 그린 상태 초기화
	/// </summary>
	void ResetDrawed()
	{
		m_isDrawed = false;
	}

	/// <summary>
	/// 창 너비 반환 (px)
	/// </summary>
	int32_t GetWidth()
	{
		return m_width;
	}

	/// <summary>
	/// 창 높이 반환 (px)
	/// </summary>
	int32_t GetHeight()
	{
		return m_height;
	}

private:
	/// <summary>
	/// 드로우 스레드
	/// </summary>
	void DrawThread();

private:
	int32_t                      m_rendererCount      = 0;
	std::vector<IERenderer>      m_renderers;
	SDL_Window*                  m_sdlWindow          = nullptr;
	int32_t                      m_width              = 0;
	int32_t                      m_height             = 0;
	std::unique_ptr<std::thread> m_drawThread;
	bool                         m_drawThreadIsRunning = false;
	bool                         m_isDrawed            = false;
};
