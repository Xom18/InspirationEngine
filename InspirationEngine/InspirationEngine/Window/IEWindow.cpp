#include <iostream>
#include <vector>
#include <deque>
#include <thread>
#include <SDL/SDL.h>
#include "InspirationEngine.h"

IEWindow::IEWindow()
{
	reset();
};
IEWindow::~IEWindow()
{
	reset();
}

void IEWindow::close()
{
	reset();
}

void IEWindow::resizeRenderer()
{
	//실제 사용하는 랜더러 생성(UI스크린하고 게임스크린이 해상도가 다른경우가 있기때문에 여러개일 수 있다)
	SDL_Surface* lpSurface = SDL_GetWindowSurface(m_sdlWindow);

	if (!lpSurface)
		return;

	//창 크기 다시 설정
	m_width = lpSurface->w;
	m_height = lpSurface->h;

	//기존에 랜더러가 있으면 논리크기는 따로 빼두고 삭제
	for (size_t i = 0; i < m_renderers.size(); ++i)
	{
		if (m_renderers[i].m_renderer != nullptr)
			SDL_DestroyRenderer(m_renderers[i].m_renderer);
		m_renderers[i].m_renderer = SDL_CreateSoftwareRenderer(lpSurface);
		m_renderers[i].m_window = this;
		SDL_SetRenderDrawColor(m_renderers[i].m_renderer, 0, 0, 0, 0xFF);

		setRendererLogicalSize(i, m_renderers[i].m_logicalWidth, m_renderers[i].m_logicalHeight);
	}
}

void IEWindow::reset()
{
	if (m_drawThread)
	{
		m_drawThread->join();
		m_drawThread.reset();
	}

	if (m_sdlWindow != nullptr)
		SDL_DestroyWindow(m_sdlWindow);

	m_sdlWindow = nullptr;
	m_width = 0;
	m_height = 0;
	m_rendererCount = 0;
	m_renderers.clear();
}

bool IEWindow::createWindow(const char* title, int32_t width, int32_t height, int32_t x, int32_t y, int32_t windowFlag, int32_t rendererCount)
{
	//이미 창이 있다
	if (m_sdlWindow != nullptr)
		return false;

	//창 생성, 랜더러 생성
	m_sdlWindow = SDL_CreateWindow(title, x, y, width, height, windowFlag);

	if (m_sdlWindow == nullptr)
		return false;

	m_width = width;
	m_height = height;
	m_rendererCount = rendererCount;
	m_renderers.resize(m_rendererCount);
	resizeRenderer();

	return true;
}

void IEWindow::render()
{
	SDL_UpdateWindowSurface(m_sdlWindow);

	//그렸으니 싹 지워준다
	for (size_t i = 0; i < m_renderers.size(); ++i)
		SDL_RenderClear(m_renderers[i].m_renderer);
}

bool IEWindow::closed()
{
	if (m_sdlWindow == nullptr)
		return true;
	else
		return false;
}

void IEWindow::setRendererLogicalSize(size_t rendererIndex, int32_t width, int32_t height)
{
	if (m_renderers.size() <= rendererIndex)
		return;

	IERenderer* lpRenderer = &m_renderers[rendererIndex];
	lpRenderer->m_logicalWidth = width;
	lpRenderer->m_logicalHeight = height;
	SDL_RenderSetLogicalSize(lpRenderer->m_renderer, lpRenderer->m_logicalWidth, lpRenderer->m_logicalHeight);
	lpRenderer->m_scaleFactor = 1;

	//화면크기 따라가는거
	if (lpRenderer->m_logicalWidth == 0 && lpRenderer->m_logicalHeight == 0)
		return;

	//화면 좌우가 잘리도록 되있기때문에 실제로 그려지는 위치를 기억
	double dWindowRatio = static_cast<double>(m_width) / m_height;
	double dRenderRatio = static_cast<double>(lpRenderer->m_logicalWidth) / lpRenderer->m_logicalHeight;

	if (dWindowRatio > dRenderRatio)
	{//좌우에 여백있는거
		int32_t iBlank = m_width - static_cast<int32_t>(m_width / dWindowRatio);
		lpRenderer->m_x = static_cast<int32_t>(iBlank * 0.5);
		lpRenderer->m_y = 0;
		lpRenderer->m_w = m_width - iBlank;
		lpRenderer->m_h = m_height;
		lpRenderer->m_scaleFactor = static_cast<double>(lpRenderer->m_logicalHeight) / m_height;
	}
	else
	{//상하에 여백있는거
		int32_t iBlank = static_cast<int32_t>(m_height * (dRenderRatio - dWindowRatio));
		lpRenderer->m_x = 0;
		lpRenderer->m_y = static_cast<int32_t>(iBlank * 0.5);
		lpRenderer->m_w = m_width;
		lpRenderer->m_h = m_height - iBlank;
		lpRenderer->m_scaleFactor = static_cast<double>(lpRenderer->m_logicalWidth) / m_width;
	}
}

void IEWindow::drawThread()
{
	std::condition_variable* lpWaiter = IECore::getDrawWaiter();
	while (true)
	{
		std::mutex mtxWaiter;
		std::unique_lock<std::mutex> lkWaiter(mtxWaiter);

		//그릴 때 까지 대기
		lpWaiter->wait(lkWaiter, [&] {
			return m_isDrawed == false || !IECore::isRunning();
			});

		//엔진이 멈췄다
		if (!IECore::isRunning())
			return;

		//창이 숨겨져있다
		if (isWindowHide())
		{
			IECore::increaseDrawCompleteCount();
			m_isDrawed = true;
			continue;
		}

		draw();
		render();
		m_isDrawed = true;
		IECore::increaseDrawCompleteCount();
	}
}

void IEWindow::beginDrawThread()
{
	if (m_drawThread != nullptr)
		return;

	m_drawThread = std::make_unique<std::thread>([&]() { drawThread(); });
}
