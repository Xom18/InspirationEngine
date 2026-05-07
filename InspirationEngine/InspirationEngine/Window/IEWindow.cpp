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

	if (lpSurface == nullptr)
		return;

	//창 크기 다시 설정
	m_width = lpSurface->w;
	m_height = lpSurface->h;

	for (size_t i = 0; i < m_renderers.size(); ++i)
	{
		if (m_renderers[i].m_renderer != nullptr)
			SDL_DestroyRenderer(m_renderers[i].m_renderer);
		m_renderers[i].m_renderer = SDL_CreateSoftwareRenderer(lpSurface);
		m_renderers[i].m_window = this;
		m_renderers[i].m_w = m_width;
		m_renderers[i].m_h = m_height;
		SDL_SetRenderDrawColor(m_renderers[i].m_renderer, 0, 0, 0, 0xFF);
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
