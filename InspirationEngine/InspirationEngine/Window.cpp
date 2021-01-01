#include <SDL/SDL.h>
#include <iostream>
#include <vector>
#include <deque>
#include <thread>
#include "MacroDefine.h"
#include "InspirationEngine.h"
#include "Window.h"

cWindow::cWindow()
{
	reset();
};
cWindow::~cWindow()
{
	reset();
}

void cWindow::close()
{
	reset();
}

void cWindow::resizeRenderer()
{
	//실제 사용하는 랜더러 생성(UI스크린하고 게임스크린이 해상도가 다른경우가 있기때문에 여러개일 수 있다)
	SDL_Surface* lpSurface = SDL_GetWindowSurface(m_pSDLWindow);

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

void cWindow::reset()
{
	for(int i = 0; i < (int)m_vecRenderer.size(); ++i)
		SDL_DestroyRenderer(m_vecRenderer[i]);
	m_vecRenderer.clear();
	SDL_DestroyWindow(m_pSDLWindow);
	m_pSDLWindow = nullptr;
	m_iWidth = 0;
	m_iHeight = 0;
	m_iRendererCount = 0;
	m_lpEngine = nullptr;
	m_pDrawThread = nullptr;
}

bool cWindow::createWindow(InspirationEngine* _lpEngine, const char* _csTitle, int _Width, int _Height, int _iX, int _iY, int _iWindowFlag, int _iRendererCount)
{
	//이미 창이 있다
	if(m_pSDLWindow)
		return false;

	//창 생성, 랜더러 생성
	m_pSDLWindow = SDL_CreateWindow(_csTitle, _iX, _iY, _Width, _Height, _iWindowFlag);

	if(!m_pSDLWindow)
		return false;

	m_iWidth = _Width;
	m_iHeight = _Height;
	m_iRendererCount = _iRendererCount;
	m_vecRenderer.reserve(m_iRendererCount);
	m_lpEngine = _lpEngine;
	resizeRenderer();

	return true;
}

void cWindow::render()
{
	SDL_UpdateWindowSurface(m_pSDLWindow);

	//그렸으니 싹 지워준다
	for(int i = 0; i < (int)m_vecRenderer.size(); ++i)
		SDL_RenderClear(m_vecRenderer[i]);
}

bool cWindow::closed()
{
	if (m_pSDLWindow == nullptr)
		return true;
	else
		return false;
}

void cWindow::setRendererLogicalSize(int _iRendererIndex, int _iWidth, int _iHeight)
{
	if((int)m_vecRenderer.size() <= _iRendererIndex)
		return;

	SDL_RenderSetLogicalSize(m_vecRenderer[_iRendererIndex], _iWidth, _iHeight);
}

void cWindow::drawBuffer(int* _lpBuffer, int _iRendererIndex, int _iBufferWidth, int _iBufferHeight, int _iX, int _iY, SDL_BlendMode _BlendMode, double _dWidthPercent, double _dHeightPercent, double _dAngle, SDL_Point* _lpPivot, SDL_RendererFlip _Flip)
{
	if((int)m_vecRenderer.size() <= _iRendererIndex)
		return;

	SDL_Texture* pTexture = SDL_CreateTexture(m_vecRenderer[_iRendererIndex], SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, _iBufferWidth, _iBufferHeight);
	SDL_SetTextureBlendMode(pTexture, _BlendMode);
	SDL_UpdateTexture(pTexture, NULL, _lpBuffer, _iBufferWidth * sizeof(Uint32));
	drawTexture(pTexture, _iRendererIndex, _iX, _iY, _dWidthPercent, _dHeightPercent, _dAngle, _lpPivot, _Flip);
	SDL_DestroyTexture(pTexture);
}

void cWindow::drawTexture(SDL_Texture* _lpTexture, int _iRendererIndex, int _iX, int _iY, double _dWidthPercent, double _dHeightPercent, double _dAngle, SDL_Point* _lpPivot, SDL_RendererFlip _Flip)
{
	if((int)m_vecRenderer.size() <= _iRendererIndex)
		return;

	//텍스쳐 정보 받아오기
	int iWidth = 0;
	int iHeight = 0;
	if(SDL_QueryTexture(_lpTexture, NULL, NULL, &iWidth, &iHeight) == -1)
		return;

	//텍스쳐 내에서의 위치
	SDL_Rect SrcRect;
	SrcRect.x = 0;
	SrcRect.y = 0;
	SrcRect.w = iWidth;
	SrcRect.h = iHeight;

	//화면에서의 위치
	SDL_Rect DestRect;
	DestRect.x = _iX;
	DestRect.y = _iY;
	DestRect.w = iWidth;
	DestRect.h = iHeight;
	if(_dWidthPercent != 100.0)
		DestRect.w = (int)(iWidth * _dWidthPercent * 0.01);
	if(_dHeightPercent != 100.0)
		DestRect.h = (int)(iHeight * _dHeightPercent * 0.01);

	SDL_RenderCopyEx(m_vecRenderer[_iRendererIndex], _lpTexture, &SrcRect, &DestRect, _dAngle, _lpPivot, _Flip);
}

void cWindow::drawThread()
{
	draw();
	render();
}

void cWindow::beginDrawThread()
{
	if(m_pDrawThread != nullptr)
		return;

	m_pDrawThread = new std::thread([&]() {drawThread(); });
}