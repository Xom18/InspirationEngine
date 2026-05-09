#include <iostream>
#include <vector>
#include <deque>
#include <thread>
#include "InspirationEngine.h"

IEWindow::IEWindow()
{
	Reset();
};
IEWindow::~IEWindow()
{
	Reset();
}

void IEWindow::Close()
{
	Reset();
}

void IEWindow::ResizeRenderer()
{
	int32_t w = 0, h = 0;
	SDL_GetWindowSize(m_sdlWindow, &w, &h);

	// 크기 변화 없고 렌더러가 이미 있으면 재생성 방지 (텍스처 무효화 방지)
	if (w == m_width && h == m_height
		&& !m_renderers.empty() && m_renderers[0].m_renderer != nullptr)
		return;

	m_width  = w;
	m_height = h;

	// 기존 렌더러 파괴 (모든 슬롯이 같은 포인터를 공유하므로 첫 번째만 파괴)
	if (!m_renderers.empty() && m_renderers[0].m_renderer != nullptr)
	{
		SDL_DestroyRenderer(m_renderers[0].m_renderer);
		for (auto& r : m_renderers)
			r.m_renderer = nullptr;
	}

	SDL_Renderer* sdlRenderer = SDL_CreateRenderer(m_sdlWindow, "software");
	if (sdlRenderer == nullptr)
		return;

	SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 0xFF);

	// 모든 슬롯이 같은 SDL_Renderer를 공유 (소프트웨어 렌더러는 창당 하나)
	for (auto& r : m_renderers)
	{
		r.m_renderer = sdlRenderer;
		r.m_window   = this;
		r.m_w        = m_width;
		r.m_h        = m_height;
	}
}

void IEWindow::Reset()
{
	if (m_drawThread != nullptr)
	{
		m_drawThread->join();
		m_drawThread.reset();
	}

	// 렌더러 먼저 파괴 (슬롯이 모두 같은 포인터이므로 첫 번째만)
	if (!m_renderers.empty() && m_renderers[0].m_renderer != nullptr)
		SDL_DestroyRenderer(m_renderers[0].m_renderer);
	m_renderers.clear();

	if (m_sdlWindow != nullptr)
		SDL_DestroyWindow(m_sdlWindow);

	m_sdlWindow     = nullptr;
	m_width         = 0;
	m_height        = 0;
	m_rendererCount = 0;
}

bool IEWindow::CreateWindow(const char* title, int32_t width, int32_t height, int32_t x, int32_t y, SDL_WindowFlags windowFlag, int32_t rendererCount)
{
	//이미 창이 있다
	if (m_sdlWindow != nullptr)
		return false;

	//창 생성, 랜더러 생성
	m_sdlWindow = SDL_CreateWindow(title, width, height, static_cast<SDL_WindowFlags>(windowFlag));
	if (m_sdlWindow != nullptr && (x != SDL_WINDOWPOS_CENTERED || y != SDL_WINDOWPOS_CENTERED))
		SDL_SetWindowPosition(m_sdlWindow, x, y);

	if (m_sdlWindow == nullptr)
		return false;

	m_width = width;
	m_height = height;
	m_rendererCount = rendererCount;
	m_renderers.resize(m_rendererCount);
	ResizeRenderer();

	return true;
}

void IEWindow::Render()
{
	if (m_renderers.empty() || m_renderers[0].m_renderer == nullptr)
		return;

	SDL_RenderPresent(m_renderers[0].m_renderer);

	//그렸으니 싹 지워준다 (슬롯이 모두 같은 렌더러이므로 한 번만)
	SDL_RenderClear(m_renderers[0].m_renderer);
}

bool IEWindow::Closed()
{
	if (m_sdlWindow == nullptr)
		return true;
	else
		return false;
}

void IEWindow::DrawThread()
{
	std::condition_variable* waiter = IECore::GetDrawWaiter();
	while (true)
	{
		std::mutex mtxWaiter;
		std::unique_lock<std::mutex> lkWaiter(mtxWaiter);

		//그릴 때 까지 대기
		waiter->wait(lkWaiter, [&] {
			return m_isDrawed == false || !IECore::IsRunning();
			});

		//엔진이 멈췄다
		if (!IECore::IsRunning())
			return;

		//창이 숨겨져있다
		if (IsWindowHide())
		{
			IECore::IncreaseDrawCompleteCount();
			m_isDrawed = true;
			continue;
		}

		Draw();
		Render();
		m_isDrawed = true;
		IECore::IncreaseDrawCompleteCount();
	}
}

void IEWindow::BeginDrawThread()
{
	if (m_drawThread != nullptr)
		return;

	m_drawThread = std::make_unique<std::thread>([&]() { DrawThread(); });
}
