#pragma once

class cDisplay	//화면 출력 클래스, 랜더 스레드화 필요
{
public:
	cDisplay();
	~cDisplay();

	/// <summary>
	/// 창 생성
	/// </summary>
	/// <param name="_lpTitle">상단 타이틀</param>
	/// <param name="_Width">창 가로 크기</param>
	/// <param name="_Height">창 세로 크기</param>
	/// <param name="_iRendererCount">랜더러 개수</param>
	/// <param name="_iWindowFlag">창 옵션</param>
	/// <returns>성공 true / 실패 false</returns>
	bool			createWindow(char* _lpTitle, int _Width, int _Height, int _iWindowFlag = 0, int _iRendererCount = 1);

	/// <summary>
	/// 창이 닫혀있는지 여부
	/// </summary>
	/// <returns>닫혀있으면 true / 열려있으면 false</returns>
	bool			closed();

	/// <summary>
	/// 창 닫기
	/// </summary>
	void			close();

	/// <summary>
	/// 랜더러 크기를 현재 창에 맞게 재지정 해주는 함수
	/// </summary>
	void			resizeRenderer();

	/// <summary>
	/// 갖고있는 변수 초기화(창 닫힘)
	/// </summary>
	void			reset();

	/// <summary>
	/// 랜더러 포인터 받아오기
	/// </summary>
	/// <param name="_iRendererIndex">랜더러 인덱스</param>
	/// <returns>랜더러 포인터 / 실패시 nullptr</returns>
	SDL_Renderer*	getRenderer(int _iRendererIndex);

	/// <summary>
	/// 창 포인터 받아오기
	/// </summary>
	/// <returns>창 포인터</returns>
	SDL_Window*		getWindow();

	/// <summary>
	/// 창 랜더러 호출
	/// </summary>
	void			render();

	/// <summary>
	/// 랜더러의 논리적 해상도 설정
	/// </summary>
	/// <param name="_iRendererIndex">변경 할 랜더러 인덱스</param>
	/// <param name="_iWidth">너비</param>
	/// <param name="_iHeight">높이</param>
	void			setRendererLogicalSize(int _iRendererIndex, int _iWidth, int _iHeight);

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
	void			drawBuffer(int* _lpBuffer, int _iRendererIndex, int _iBufferWidth, int _iBufferHeight, int _iX, int _iY, SDL_BlendMode _BlendMode = SDL_BLENDMODE_BLEND, double _dWidthPercent = 100, double _dHeightPercent = 100, double _dAngle = 0, SDL_Point* _lpPivot = nullptr, SDL_RendererFlip _Flip = SDL_FLIP_NONE);

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
	void			drawTexture(SDL_Texture* _lpTexture, int _iRendererIndex, int _iX, int _iY, double _dWidthPercent = 100, double _dHeightPercent = 100, double _dAngle = 0, SDL_Point* _lpPivot = nullptr, SDL_RendererFlip _Flip = SDL_FLIP_NONE);

public:

private:
	int							m_iRendererCount;	//랜더러 개수
	std::vector<SDL_Renderer*>	m_vecRenderer;		//소프트웨어 랜더러
	SDL_Window*					m_pWindow;			//창
	int							m_iWidth;			//너비
	int							m_iHeight;			//높이
};