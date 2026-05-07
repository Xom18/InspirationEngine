#pragma once

class IEWindow;
class IERenderer
{
public:
	IEWindow* m_window;			//얘가 종속되있는 윈도우
	SDL_Renderer* m_renderer;	//랜더러
	int32_t m_w;				//창에서 랜더러 너비
	int32_t m_h;				//창에서 랜더러 높이

private:

public:

	IERenderer()
	{
		m_window = nullptr;
		m_renderer = nullptr;
		m_w = 0;
		m_h = 0;
	}

	~IERenderer()
	{
		if (m_renderer != nullptr)
			SDL_DestroyRenderer(m_renderer);
	}

	/// <summary>
	/// 렌더러 초기화
	/// </summary>
	void reset()
	{
		if (m_renderer != nullptr)
			SDL_DestroyRenderer(m_renderer);
		m_renderer = nullptr;
		m_w = 0;
		m_h = 0;
	}

	/// <summary>
	/// 사각형 그리기
	/// </summary>
	/// <param name="color">색상</param>
	/// <param name="x">좌측 X</param>
	/// <param name="y">상단 Y</param>
	/// <param name="width">너비</param>
	/// <param name="height">높이</param>
	/// <param name="blendMode">블렌딩 모드</param>
	/// <param name="angle">회전 각도 (도)</param>
	/// <param name="pivot">회전 기준점 (nullptr이면 중앙)</param>
	void drawRect(SDL_Color color, int32_t x, int32_t y, int32_t width, int32_t height, SDL_BlendMode blendMode = SDL_BLENDMODE_NONE, double angle = 0.0, SDL_Point* pivot = nullptr);

	/// <summary>
	/// ARGB8888 픽셀 버퍼 그리기
	/// </summary>
	/// <param name="buffer">ARGB8888 픽셀 버퍼</param>
	/// <param name="bufferWidth">버퍼 너비</param>
	/// <param name="bufferHeight">버퍼 높이</param>
	/// <param name="x">화면 X</param>
	/// <param name="y">화면 Y</param>
	/// <param name="blendMode">블렌딩 모드</param>
	/// <param name="widthPercent">너비 배율 (%)</param>
	/// <param name="heightPercent">높이 배율 (%)</param>
	/// <param name="angle">회전 각도 (도)</param>
	/// <param name="pivot">회전 기준점 (nullptr이면 중앙)</param>
	/// <param name="flip">반전 플래그</param>
	void drawBuffer(int32_t* buffer, int32_t bufferWidth, int32_t bufferHeight, int32_t x, int32_t y, SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND, double widthPercent = 100, double heightPercent = 100, double angle = 0, SDL_Point* pivot = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);

	/// <summary>
	/// 텍스쳐 그리기
	/// </summary>
	/// <param name="texture">그릴 텍스쳐</param>
	/// <param name="x">화면 X</param>
	/// <param name="y">화면 Y</param>
	/// <param name="widthPercent">너비 배율 (%)</param>
	/// <param name="heightPercent">높이 배율 (%)</param>
	/// <param name="angle">회전 각도 (도)</param>
	/// <param name="pivot">회전 기준점 (nullptr이면 중앙)</param>
	/// <param name="flip">반전 플래그</param>
	/// <param name="srcRect">텍스쳐 내 클리핑 영역 (nullptr이면 전체)</param>
	void drawTexture(SDL_Texture* texture, int32_t x, int32_t y, double widthPercent = 100, double heightPercent = 100, double angle = 0, SDL_Point* pivot = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE, const SDL_Rect* srcRect = nullptr);

	/// <summary>
	/// 내부 SDL_Renderer 반환
	/// </summary>
	SDL_Renderer* getSDLRenderer()
	{
		return m_renderer;
	}

	/// <summary>
	/// 서페이스 그리기
	/// </summary>
	/// <param name="surface">그릴 서페이스</param>
	/// <param name="x">화면 X</param>
	/// <param name="y">화면 Y</param>
	/// <param name="widthPercent">너비 배율 (%)</param>
	/// <param name="heightPercent">높이 배율 (%)</param>
	/// <param name="angle">회전 각도 (도)</param>
	/// <param name="pivot">회전 기준점 (nullptr이면 중앙)</param>
	/// <param name="flip">반전 플래그</param>
	void drawSurface(SDL_Surface* surface, int32_t x, int32_t y, double widthPercent = 100, double heightPercent = 100, double angle = 0, SDL_Point* pivot = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);

	/// <summary>
	/// 텍스트 그리기
	/// </summary>
	/// <param name="font">사용할 폰트</param>
	/// <param name="text">텍스트 (UTF-8)</param>
	/// <param name="color">글자 색상</param>
	/// <param name="x">화면 X</param>
	/// <param name="y">화면 Y</param>
	/// <param name="widthPercent">너비 배율 (%)</param>
	/// <param name="heightPercent">높이 배율 (%)</param>
	/// <param name="angle">회전 각도 (도)</param>
	/// <param name="pivot">회전 기준점 (nullptr이면 중앙)</param>
	/// <param name="flip">반전 플래그</param>
	void drawText(Font* font, const char* text, SDL_Color color, int32_t x, int32_t y, double widthPercent = 100, double heightPercent = 100, double angle = 0, SDL_Point* pivot = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);

	/// <summary>
	/// 선 그리기
	/// </summary>
	void drawLine(SDL_Color color, int32_t x1, int32_t y1, int32_t x2, int32_t y2);

	/// <summary>
	/// 현재 렌더러 화면을 PNG 파일로 저장
	/// </summary>
	/// <param name="path">저장 경로 (.png)</param>
	bool saveScreenshot(const char* path);

private:

};
