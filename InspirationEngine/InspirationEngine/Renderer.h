#pragma once

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
	void drawBuffer(int* _lpBuffer, int _iBufferWidth, int _iBufferHeight, int _iX, int _iY, SDL_BlendMode _BlendMode = SDL_BLENDMODE_BLEND, double _dWidthPercent = 100, double _dHeightPercent = 100, double _dAngle = 0, SDL_Point* _lpPivot = nullptr, SDL_RendererFlip _Flip = SDL_FLIP_NONE);

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
	void drawTexture(SDL_Texture* _lpTexture, int _iX, int _iY, double _dWidthPercent = 100, double _dHeightPercent = 100, double _dAngle = 0, SDL_Point* _lpPivot = nullptr, SDL_RendererFlip _Flip = SDL_FLIP_NONE);

	/// <summary>
	/// 서페이스 그리기
	/// </summary>
	/// <param name="SDL_Surface">서페이스</param>
	/// <param name="_iRendererIndex">랜더러 인덱스</param>
	/// <param name="_iX">X위치</param>
	/// <param name="_iY">Y위치</param>
	/// <param name="_dWidthPercent">너비(퍼센트)</param>
	/// <param name="_dHeightPercent">높이(퍼센트)</param>
	/// <param name="_dAngle">회전 각도(도)</param>
	/// <param name="_lpPivot">회전 중심</param>
	/// <param name="_Flip">상하, 좌우 반전 SDL_FLIP_HORIZONTAL / SDL_FLIP_VERTICAL</param>
	void drawSurface(SDL_Surface* _lpTexture, int _iX, int _iY, double _dWidthPercent = 100, double _dHeightPercent = 100, double _dAngle = 0, SDL_Point* _lpPivot = nullptr, SDL_RendererFlip _Flip = SDL_FLIP_NONE);

	/// <summary>
	/// 텍스트 그리기
	/// </summary>
	/// <param name="_lpFont">폰트</param>
	/// <param name="_lpText">텍스트</param>
	/// <param name="_Color">텍스트 컬러</param>
	/// <param name="_iRendererIndex">랜더러 인덱스</param>
	/// <param name="_iX">X위치</param>
	/// <param name="_iY">Y위치</param>
	/// <param name="_dWidthPercent">너비(퍼센트)</param>
	/// <param name="_dHeightPercent">높이(퍼센트)</param>
	/// <param name="_dAngle">회전 각도(도)</param>
	/// <param name="_lpPivot">회전 중심</param>
	/// <param name="_Flip">상하, 좌우 반전 SDL_FLIP_HORIZONTAL / SDL_FLIP_VERTICAL</param>
	void drawText(TTF_Font* _lpFont, const char* _lpText, SDL_Color _Color, int _iX, int _iY, double _dWidthPercent = 100, double _dHeightPercent = 100, double _dAngle = 0, SDL_Point* _lpPivot = nullptr, SDL_RendererFlip _Flip = SDL_FLIP_NONE);

private:

};
