#pragma once

class IECore;

class IEWindow	//화면 출력 클래스, 랜더 스레드화 필요
{
public:

private:
	int32_t						m_rendererCount;	//랜더러 개수
	std::vector<IERenderer>		m_renderers;		//소프트웨어 랜더러
	SDL_Window* m_sdlWindow;		//창
	int32_t						m_width;			//너비
	int32_t						m_height;			//높이

	std::unique_ptr<std::thread> m_drawThread;		//그리는 스레드
	bool						m_drawThreadIsRunning;//그리는 스레드가 도는중인지
	bool						m_isDrawed;			//이번에 그렸다

public:
	IEWindow();

	~IEWindow();

	/// <summary>
	/// 프레임 업데이트
	/// </summary>
	/// <param name="deltaTime">경과 시간 (초)</param>
	virtual void update(float deltaTime) {}

	/// <summary>
	/// 그리기
	/// </summary>
	virtual void draw() {};

	/// <summary>
	/// X버튼을 눌렀을 때
	/// </summary>
	virtual void callXButton() {};

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
	bool createWindow(const char* title, int32_t width, int32_t height, int32_t x = SDL_WINDOWPOS_CENTERED, int32_t y = SDL_WINDOWPOS_CENTERED, int32_t windowFlag = 0, int32_t rendererCount = 1);

	/// <summary>
	/// 창이 닫혀있는지 여부
	/// </summary>
	bool closed();

	/// <summary>
	/// 창 닫기
	/// </summary>
	void close();

	/// <summary>
	/// 랜더러 크기를 현재 창에 맞게 재지정 해주는 함수
	/// </summary>
	void resizeRenderer();

	/// <summary>
	/// 갖고있는 변수 초기화(창 닫힘)
	/// </summary>
	void reset();

	/// <summary>
	/// 렌더러 포인터 받아오기
	/// </summary>
	/// <param name="rendererIndex">렌더러 인덱스</param>
	IERenderer* getRenderer(int32_t rendererIndex)
	{
		if (static_cast<int32_t>(m_renderers.size()) <= rendererIndex)
			return nullptr;

		return &m_renderers[rendererIndex];
	}

	/// <summary>
	/// 창 포인터 받아오기
	/// </summary>
	SDL_Window* getSDLWindow()
	{
		return m_sdlWindow;
	};

	/// <summary>
	/// 드로우 끝난 창 랜더
	/// </summary>
	void render();

	/// <summary>
	/// 렌더러의 논리적 해상도 설정
	/// </summary>
	/// <param name="rendererIndex">렌더러 인덱스</param>
	/// <param name="width">논리 너비 (0이면 창 크기 따라감)</param>
	/// <param name="height">논리 높이 (0이면 창 크기 따라감)</param>
	void setRendererLogicalSize(size_t rendererIndex, int32_t width, int32_t height);

	/// <summary>
	/// 이 창이 하이드 상태인지
	/// </summary>
	bool isWindowHide()
	{
		return SDL_GetWindowFlags(m_sdlWindow) & SDL_WINDOW_HIDDEN;
	}

	/// <summary>
	/// 창 표시
	/// </summary>
	void showWindow()
	{
		SDL_ShowWindow(m_sdlWindow);
	}

	/// <summary>
	/// 창 숨기기
	/// </summary>
	void hideWindow()
	{
		SDL_HideWindow(m_sdlWindow);
	}

	/// <summary>
	/// 드로우 스레드 호출
	/// </summary>
	void beginDrawThread();

	/// <summary>
	/// 스레드 종료 대기
	/// </summary>
	void joinDrawThread()
	{
		if (m_drawThread == nullptr)
			return;
		m_drawThread->join();
	}

	/// <summary>
	/// 스레드 죽이기
	/// </summary>
	void deleteThread()
	{
		m_drawThread.reset();
	}

	/// <summary>
	/// 그린 상태 초기화
	/// </summary>
	void resetDrawed()
	{
		m_isDrawed = false;
	}

	/// <summary>
	/// 스크린 좌표를 렌더러 논리 좌표로 변환
	/// </summary>
	/// <param name="rendererIndex">렌더러 인덱스</param>
	/// <param name="screenX">스크린 X</param>
	/// <param name="screenY">스크린 Y</param>
	IVector2 screenPosToRenderPos(int32_t rendererIndex, int32_t screenX, int32_t screenY)
	{
		IVector2 Pos;
		Pos.m_x = screenX;
		Pos.m_y = screenY;

		const IERenderer* lpRenderer = getRenderer(rendererIndex);
		if (lpRenderer == nullptr)
			return Pos;

		if (lpRenderer->m_logicalWidth == 0 && lpRenderer->m_logicalHeight == 0)
			return Pos;

		//상하 또는 좌우 여백이 있는거 처리
		screenX = screenX - lpRenderer->m_x;
		screenY = screenY - lpRenderer->m_y;

		//픽셀 스케일 배수 처리
		Pos.m_x = static_cast<int32_t>(screenX * lpRenderer->m_scaleFactor);
		Pos.m_y = static_cast<int32_t>(screenY * lpRenderer->m_scaleFactor);
		return Pos;
	}

	/// <summary>
	/// 렌더러 논리 좌표를 스크린 좌표로 변환
	/// </summary>
	/// <param name="rendererIndex">렌더러 인덱스</param>
	/// <param name="screenX">렌더러 논리 X</param>
	/// <param name="screenY">렌더러 논리 Y</param>
	IVector2 renderPosToScreenPos(int32_t rendererIndex, int32_t screenX, int32_t screenY)
	{
		IVector2 Pos;
		Pos.m_x = screenX;
		Pos.m_y = screenY;

		const IERenderer* lpRenderer = getRenderer(rendererIndex);
		if (lpRenderer == nullptr)
			return Pos;

		if (lpRenderer->m_logicalWidth == 0 && lpRenderer->m_logicalHeight == 0)
			return Pos;

		//픽셀 스케일 배수 처리
		Pos.m_x = static_cast<int32_t>(screenX / lpRenderer->m_scaleFactor) + lpRenderer->m_x;
		Pos.m_y = static_cast<int32_t>(screenY / lpRenderer->m_scaleFactor) + lpRenderer->m_y;

		return Pos;
	}

	/// <summary>
	/// 창 너비 반환 (px)
	/// </summary>
	int32_t getWidth()
	{
		return m_width;
	}

	/// <summary>
	/// 창 높이 반환 (px)
	/// </summary>
	int32_t getHeight()
	{
		return m_height;
	}

private:

	/// <summary>
	/// 드로우 스레드
	/// </summary>
	void drawThread();
};
