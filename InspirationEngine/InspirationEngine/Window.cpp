#include <SDL/SDL.h>
#include <iostream>
#include <vector>
#include <deque>
#include <thread>
#include "MacroDefine.h"
#include "InspirationEngine.h"

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

	//창 크기 다시 설정
	m_iWidth = lpSurface->w;
	m_iHeight = lpSurface->h;

	//기존에 랜더러가 있으면 논리크기는 따로 빼두고 삭제
	for(int i = 0; i < static_cast<int>(m_vecRenderer.size()); ++i)
	{
		if(m_vecRenderer[i].m_pRenderer != nullptr)
			SDL_DestroyRenderer(m_vecRenderer[i].m_pRenderer);
		m_vecRenderer[i].m_pRenderer = SDL_CreateSoftwareRenderer(lpSurface);
		SDL_SetRenderDrawColor(m_vecRenderer[i].m_pRenderer, 0, 0, 0, 0xFF);

		setRendererLogicalSize(i, m_vecRenderer[i].m_iLogicalWidth, m_vecRenderer[i].m_iLogicalHeight);
	}
}

void cWindow::reset()
{
	m_vecRenderer.clear();
	SDL_DestroyWindow(m_pSDLWindow);
	m_pSDLWindow = nullptr;
	m_iWidth = 0;
	m_iHeight = 0;
	m_iRendererCount = 0;
	m_pDrawThread = nullptr;
}

bool cWindow::createWindow(const char* _csTitle, int _Width, int _Height, int _iX, int _iY, int _iWindowFlag, int _iRendererCount)
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
	m_vecRenderer.resize(m_iRendererCount);
	resizeRenderer();

	return true;
}

void cWindow::render()
{
	SDL_UpdateWindowSurface(m_pSDLWindow);

	//그렸으니 싹 지워준다
	for(size_t i = 0; i < m_vecRenderer.size(); ++i)
		SDL_RenderClear(m_vecRenderer[i].m_pRenderer);
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
	if(static_cast<int>(m_vecRenderer.size()) <= _iRendererIndex)
		return;

	cRenderer* lpRenderer = &m_vecRenderer[_iRendererIndex];
	lpRenderer->m_iLogicalWidth = _iWidth;
	lpRenderer->m_iLogicalHeight = _iHeight;
	SDL_RenderSetLogicalSize(lpRenderer->m_pRenderer, lpRenderer->m_iLogicalWidth, lpRenderer->m_iLogicalHeight);
	lpRenderer->m_dScaleFactor = 1;

	//화면크기 따라가는거
	if(lpRenderer->m_iLogicalWidth == 0 && lpRenderer->m_iLogicalHeight == 0)
		return;

	//화면 좌우가 잘리도록 되있기때문에 실제로 그려지는 위치를 기억
	double dWindowRatio = static_cast<double>(m_iWidth) / m_iHeight;
	double dRenderRatio = static_cast<double>(lpRenderer->m_iLogicalWidth) / lpRenderer->m_iLogicalHeight;

	if(dWindowRatio > dRenderRatio)
	{//좌우에 여백있는거
		int iBlank = m_iWidth - static_cast<int>(m_iWidth / dWindowRatio);
		lpRenderer->m_iX = static_cast<int>(iBlank * 0.5);
		lpRenderer->m_iY = 0;
		lpRenderer->m_iW = m_iWidth - iBlank;
		lpRenderer->m_iH = m_iHeight;
		lpRenderer->m_dScaleFactor = static_cast<double>(lpRenderer->m_iLogicalHeight) / m_iHeight;
	}
	else
	{//상하에 여백있는거
		int iBlank = static_cast<int>(m_iHeight * (dRenderRatio - dWindowRatio));
		lpRenderer->m_iX = 0;
		lpRenderer->m_iY = static_cast<int>(iBlank * 0.5);
		lpRenderer->m_iW = m_iWidth;
		lpRenderer->m_iH = m_iHeight - iBlank;
		lpRenderer->m_dScaleFactor = static_cast<double>(lpRenderer->m_iLogicalWidth) / m_iWidth;
	}
}

void cWindow::drawThread()
{
	std::condition_variable* lpWaiter = cIECore::getDrawWaiter();
	while(true)
	{
		std::mutex mtxWaiter;
		std::unique_lock<std::mutex> lkWaiter(mtxWaiter);
		
		//그릴 때 까지 대기
		lpWaiter->wait(lkWaiter, [&] {
			return m_bIsDrawed == false || !cIECore::isRunning();
		});

		//엔진이 멈췄다
		if(!cIECore::isRunning())
			return;

		//창이 숨겨져있다
		if(isWindowHide())
		{
			cIECore::increaseDrawCompleteCount();
			m_bIsDrawed = true;
			continue;
		}

		draw();
		render();
		cIECore::increaseDrawCompleteCount();
		m_bIsDrawed = true;
	}
}

void cWindow::beginDrawThread()
{
	if(m_pDrawThread != nullptr)
		return;

	m_pDrawThread = new std::thread([&]() {drawThread(); });
}