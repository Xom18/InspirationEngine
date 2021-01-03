#pragma once

class InspirationEngine;

class cRenderer
{
public:
	SDL_Renderer* m_pRenderer;	//랜더러
	int m_iLogicalWidth;		//논리너비
	int m_iLogicalHeight;		//논리높이
	int m_iX;					//창에서 랜더러 위치
	int m_iY;					//창에서 랜더러 위치
	int m_iW;					//창에서 랜더러 너비
	int m_iH;					//창에서 랜더러 높이
	double m_dScaleFactor;		//랜더러 마우스위치 스케일 계수

private:

public:

	cRenderer()
	{
		m_pRenderer = nullptr;
		m_iLogicalWidth = 0;
		m_iLogicalHeight = 0;
		m_iX = 0;
		m_iY = 0;
		m_iW = 0;
		m_iH = 0;
		m_dScaleFactor = 0.0;
	}

	~cRenderer()
	{
		if(m_pRenderer != nullptr)
			SDL_DestroyRenderer(m_pRenderer);
	}

	void reset()
	{
		if(m_pRenderer != nullptr)
			SDL_DestroyRenderer(m_pRenderer);
		m_pRenderer = nullptr;
		m_iLogicalWidth = 0;
		m_iLogicalHeight = 0;
		m_iX = 0;
		m_iY = 0;
		m_iW = 0;
		m_iH = 0;
		m_dScaleFactor = 0.0;
	}

private:

};

class cWindow	//화면 출력 클래스, 랜더 스레드화 필요
{
public:

private:

	int							m_iRendererCount;	//랜더러 개수
	std::vector<cRenderer>		m_vecRenderer;		//소프트웨어 랜더러
	SDL_Window*					m_pSDLWindow;		//창
	int							m_iWidth;			//너비
	int							m_iHeight;			//높이

	std::thread*				m_pDrawThread;		//그리는 스레드
	bool						m_bDrawThreadIsRunning;//그리는 스레드가 도는중인지
	bool						m_bIsDrawed;		//이번에 그렸다

protected:
	InspirationEngine* m_lpEngine;//이것의 소유주

public:
	cWindow();

	~cWindow();

	/// <summary>
	/// 그리기
	/// </summary>
	virtual void draw() = 0;

	/// <summary>
	/// X버튼을 눌렀을 때
	/// </summary>
	virtual void callXButton() = 0;

	/// <summary>
	/// 창 생성
	/// </summary>
	/// <param name="_lpTitle">상단 타이틀</param>
	/// <param name="_Width">창 가로 크기</param>
	/// <param name="_Height">창 세로 크기</param>
	/// <param name="_iRendererCount">랜더러 개수</param>
	/// <param name="_iWindowFlag">창 옵션</param>
	/// <returns>성공 true / 실패 false</returns>
	bool createWindow(InspirationEngine* _lpEngine, const char* _csTitle, int _Width, int _Height, int _iX = SDL_WINDOWPOS_CENTERED, int _iY = SDL_WINDOWPOS_CENTERED, int _iWindowFlag = 0, int _iRendererCount = 1);

	/// <summary>
	/// 창이 닫혀있는지 여부
	/// </summary>
	/// <returns>닫혀있으면 true / 열려있으면 false</returns>
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
	/// 랜더러 포인터 받아오기
	/// </summary>
	/// <param name="_iRendererIndex">랜더러 인덱스</param>
	/// <returns>랜더러 포인터 / 실패시 nullptr</returns>
	cRenderer* getRenderer(int _iRendererIndex)
	{
		if((int)m_vecRenderer.size() <= _iRendererIndex)
			return nullptr;

		return &m_vecRenderer[_iRendererIndex];
	}

	/// <summary>
	/// 창 포인터 받아오기
	/// </summary>
	/// <returns>창 포인터</returns>
	SDL_Window* getSDLWindow()
	{
		return m_pSDLWindow;
	};

	/// <summary>
	/// 드로우 끝난 창 랜더
	/// </summary>
	void render();

	/// <summary>
	/// 랜더러의 논리적 해상도 설정
	/// </summary>
	/// <param name="_iRendererIndex">변경 할 랜더러 인덱스</param>
	/// <param name="_iWidth">너비</param>
	/// <param name="_iHeight">높이</param>
	void setRendererLogicalSize(int _iRendererIndex, int _iWidth, int _iHeight);

	/// <summary>
	/// 버퍼에 있는 이미지 그리기
	/// </summary>
	/// <param name="_lpBuffer">이미지 데이터 버퍼</param>
	/// <param name="_iRendererIndex">랜더러 인덱스</param>
	/// <param name="_iBufferWidth">버퍼 너비</param>
	/// <param name="_iBufferHeight">버퍼 높이</param>
	/// <param name="_iX">X위치</param>
	/// <param name="_iY">Y위치</param>
	/// <param name="_BlendMode">블랜드 모드</param>
	/// <param name="_dWidthPercent">너비(퍼센트)</param>
	/// <param name="_dHeightPercent">높이(퍼센트)</param>
	/// <param name="_dAngle">회전 각도(도)</param>
	/// <param name="_lpPivot">회전 중심</param>
	/// <param name="_Flip">상하, 좌우 반전 SDL_FLIP_HORIZONTAL / SDL_FLIP_VERTICAL</param>
	void drawBuffer(int* _lpBuffer, int _iRendererIndex, int _iBufferWidth, int _iBufferHeight, int _iX, int _iY, SDL_BlendMode _BlendMode = SDL_BLENDMODE_BLEND, double _dWidthPercent = 100, double _dHeightPercent = 100, double _dAngle = 0, SDL_Point* _lpPivot = nullptr, SDL_RendererFlip _Flip = SDL_FLIP_NONE);

	/// <summary>
	/// 텍스쳐 이미지 그리기
	/// </summary>
	/// <param name="_lpTexture">텍스쳐</param>
	/// <param name="_iRendererIndex">랜더러 인덱스</param>
	/// <param name="_iX">X위치</param>
	/// <param name="_iY">Y위치</param>
	/// <param name="_dWidthPercent">너비(퍼센트)</param>
	/// <param name="_dHeightPercent">높이(퍼센트)</param>
	/// <param name="_dAngle">회전 각도(도)</param>
	/// <param name="_lpPivot">회전 중심</param>
	/// <param name="_Flip">상하, 좌우 반전 SDL_FLIP_HORIZONTAL / SDL_FLIP_VERTICAL</param>
	void drawTexture(SDL_Texture* _lpTexture, int _iRendererIndex, int _iX, int _iY, double _dWidthPercent = 100, double _dHeightPercent = 100, double _dAngle = 0, SDL_Point* _lpPivot = nullptr, SDL_RendererFlip _Flip = SDL_FLIP_NONE);

	/// <summary>
	/// 이 창이 하이드 상태인지
	/// </summary>
	/// <returns>하이드 상태면 true / 아니면 false</returns>
	bool isWindowHide()
	{
		return SDL_GetWindowFlags(m_pSDLWindow) & SDL_WINDOW_HIDDEN;
	}

	/// <summary>
	/// 창 표시
	/// </summary>
	void showWindow()
	{
		SDL_ShowWindow(m_pSDLWindow);
	}

	/// <summary>
	/// 창 숨기기
	/// </summary>
	void hideWindow()
	{
		SDL_HideWindow(m_pSDLWindow);
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
		if(m_pDrawThread == nullptr)
			return;
		m_pDrawThread->join();
	}

	/// <summary>
	/// 스레드 죽이기
	/// </summary>
	void deleteThread()
	{
		KILL(m_pDrawThread);
	}

	/// <summary>
	/// 이번에 이미 그렸는지
	/// </summary>
	/// <returns>이미 그렸으면 true / 아니면 false</returns>
	bool isDrawed()
	{
		return m_bIsDrawed;
	}

	/// <summary>
	/// 그린 상태 초기화
	/// </summary>
	void resetDrawed()
	{
		m_bIsDrawed = false;
	}

	/// <summary>
	/// 랜더상에서 X, Y위치 받아오기
	/// </summary>
	/// <param name="_iRendererIndex">랜더러 번호</param>
	/// <param name="_iScreenX">화면상 X</param>
	/// <param name="_iScreenY">화면상 Y</param>
	/// <returns></returns>
	cIVector2 getRenderPos(int _iRendererIndex, int _iScreenX, int _iScreenY)
	{
		cIVector2 Pos;
		Pos.m_iX = _iScreenX;
		Pos.m_iY = _iScreenY;

		const cRenderer* lpRenderer = getRenderer(_iRendererIndex);
		if(lpRenderer == nullptr)
			return Pos;

		if(lpRenderer->m_iLogicalWidth == 0 && lpRenderer->m_iLogicalHeight == 0)
			return Pos;

		//상하 또는 좌우 여백이 있는거 처리
		_iScreenX = _iScreenX - lpRenderer->m_iX;
		_iScreenY = _iScreenY - lpRenderer->m_iY;

		//픽셀 스케일 배수 처리
		Pos.m_iX = (int)(_iScreenX * lpRenderer->m_dScaleFactor);
		Pos.m_iY = (int)(_iScreenY * lpRenderer->m_dScaleFactor);
		return Pos;
	}

private:

	/// <summary>
	/// 드로우 스레드
	/// </summary>
	void drawThread();
};