#include <SDL/SDL.h>
#include <iostream>
#include <vector>
#include <deque>
#include "MacroDefine.h"
#include "Display.h"

cDisplay::cDisplay()
{
	reset();
}

cDisplay::~cDisplay()
{
	reset();
}

void cDisplay::close()
{
	reset();
}

//창 크기 재설정
void cDisplay::resize()
{
	//실제 사용하는 랜더러 생성(UI스크린하고 게임스크린이 해상도가 다른경우가 있기때문에 여러개일 수 있다)
	SDL_Surface* lpSurface = SDL_GetWindowSurface(m_pWindow);

	if(!lpSurface)
		return;

	std::deque<IVector2> dqLogicalSize;	//논리크기 저장해두는곳

	//기존에 랜더러가 있으면 논리크기는 따로 빼두고 삭제
	if(m_vecRenderer.size())
	{
		for(int i = 0; i < (int)m_vecRenderer.size(); ++i)
		{
			IVector2 LogicalSize;
			SDL_RenderGetLogicalSize(m_vecRenderer[i], &LogicalSize.m_iX, &LogicalSize.m_iY);
			SDL_DestroyRenderer(m_vecRenderer[i]);
			dqLogicalSize.push_back(LogicalSize);
		}
		m_vecRenderer.clear();
	}

	for(int i = 0; i < m_iRendererCount; ++i)
	{
		SDL_Renderer* pRenderer = SDL_CreateSoftwareRenderer(lpSurface);
		SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 0xFF);

		//논리크기가 있을때 처리
		if(!dqLogicalSize.empty())
		{
			IVector2 LogicalSize;
			LogicalSize = dqLogicalSize.front();
			dqLogicalSize.pop_front();
			SDL_RenderSetLogicalSize(pRenderer, LogicalSize.m_iX, LogicalSize.m_iY);
		}

		m_vecRenderer.push_back(pRenderer);
	}
}

//초기화
void cDisplay::reset()
{
	for(int i = 0; i < (int)m_vecRenderer.size(); ++i)
		SDL_DestroyRenderer(m_vecRenderer[i]);
	SDL_DestroyWindow(m_pWindow);
	m_pWindow = nullptr;
	m_iWidth = 0;
	m_iHeight = 0;
	m_iRendererCount = 0;
	m_vecRenderer.clear();
}

bool cDisplay::createWindow(char * _lpTitle, int _Width, int _Height, int _iRendererCount)
{
	//이미 창이 있다
	if(m_pWindow)
		return false;

	//창 생성, 랜더러 생성
	m_pWindow = SDL_CreateWindow(_lpTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _Width, _Height, SDL_WINDOW_RESIZABLE /*SDL_WindowFlags*/);

	if(!m_pWindow)
		return false;

	m_iWidth = _Width;
	m_iHeight = _Height;
	m_iRendererCount = _iRendererCount;
	m_vecRenderer.reserve(m_iRendererCount);
	resize();

	return true;
}

void cDisplay::render()
{
	SDL_UpdateWindowSurface(m_pWindow);

	//그렸으니 싹 지워준다
	for(int i = 0; i < (int)m_vecRenderer.size(); ++i)
		SDL_RenderClear(m_vecRenderer[i]);
}

//닫혀있냐
bool cDisplay::closed()
{
	if (m_pWindow == NULL)
		return true;
	else
		return false;
}

//랜더러 받아오는거
SDL_Renderer* cDisplay::getRenderer(int _iRendererIndex)
{
	if((int)m_vecRenderer.size() <= _iRendererIndex)
		return NULL;

	return m_vecRenderer[_iRendererIndex];
}

SDL_Window* cDisplay::getWindow()
{
	return m_pWindow;
}

//논리 크기 설정
void cDisplay::setRendererLogicalSize(int _iRendererIndex, int _iWidth, int _iHeight)
{
	if((int)m_vecRenderer.size() <= _iRendererIndex)
		return;

	SDL_RenderSetLogicalSize(m_vecRenderer[_iRendererIndex], _iWidth, _iHeight);
}

//버퍼를 그리려고 하는경우
void cDisplay::draw(int* _lpBuffer, int _iRendererIndex, int _iX, int _iY, int _iWidth, int _iHeight, SDL_BlendMode _BlendMode)
{
	SDL_Rect Rect;//타일 띄울 위치
	Rect.x = _iX;
	Rect.y = _iY;
	Rect.w = _iWidth;
	Rect.h = _iHeight;
	draw(_lpBuffer, _iRendererIndex, Rect, _BlendMode);
}

//텍스쳐를 만들어서 그걸 그려줌
void cDisplay::draw(int* _lpBuffer, int _iRendererIndex, const SDL_Rect& _lpRect, SDL_BlendMode _BlendMode)
{
	if((int)m_vecRenderer.size() <= _iRendererIndex)
		return;

	SDL_Texture* pTexture = SDL_CreateTexture(m_vecRenderer[_iRendererIndex], SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, _lpRect.w, _lpRect.h);
	SDL_SetTextureBlendMode(pTexture, _BlendMode);
	SDL_UpdateTexture(pTexture, NULL, _lpBuffer, _lpRect.w * sizeof(Uint32));
	draw(pTexture, _iRendererIndex, _lpRect);
	SDL_DestroyTexture(pTexture);
}

//텍스쳐를 그리려고 하는경우
void cDisplay::draw(SDL_Texture* _lpTexture, int _iRendererIndex, int _iX, int _iY, int _iWidth, int _iHeight)
{
	SDL_Rect Rect;//타일 띄울 위치
	Rect.x = _iX;
	Rect.y = _iY;
	Rect.w = _iWidth;
	Rect.h = _iHeight;
	draw(_lpTexture, _iRendererIndex, Rect);
}

//텍스쳐를 그리려고 하는경우
void cDisplay::draw(SDL_Texture* _lpTexture, int _iRendererIndex, const SDL_Rect& _lpRect)
{
	if((int)m_vecRenderer.size() <= _iRendererIndex)
		return;

	SDL_RenderCopy(m_vecRenderer[_iRendererIndex], _lpTexture, NULL, &_lpRect);
}
