#include "../InspirationEngine.h"

void IEScrollView::ClampScroll()
{
    int32_t maxScroll = m_contentH - m_rect.h;
    if (maxScroll < 0)
        maxScroll = 0;
    if (m_scrollY < 0)
        m_scrollY = 0;
    if (m_scrollY > maxScroll)
        m_scrollY = maxScroll;
}

void IEScrollView::Update()
{
    if (m_ownerWindow == nullptr || IECore::GetMouseOnWindow() != m_ownerWindow)
        return;

    // 마우스가 뷰포트 위에 있을 때만 스크롤
    float gx = 0.0f, gy = 0.0f;
    SDL_GetGlobalMouseState(&gx, &gy);

    int32_t winX = 0, winY = 0;
    SDL_GetWindowPosition(m_ownerWindow->GetSDLWindow(), &winX, &winY);

    int32_t mx = static_cast<int32_t>(gx) - winX;
    int32_t my = static_cast<int32_t>(gy) - winY;

    bool onViewport = (mx >= m_rect.x && mx < m_rect.x + m_rect.w &&
                       my >= m_rect.y && my < m_rect.y + m_rect.h);
    if (!onViewport)
        return;

    float wheel = IECore::GetInput().GetMouseWheelY();
    if (wheel != 0.0f)
    {
        m_scrollY -= static_cast<int32_t>(wheel * kScrollSpeed);
        ClampScroll();
    }
}

void IEScrollView::Draw()
{
    IERenderer* r = GetRenderer();
    if (r == nullptr || m_contentH <= m_rect.h)
        return;

    // 스크롤바 트랙
    int32_t trackX = m_rect.x + m_rect.w - kScrollbarW;
    int32_t trackY = m_rect.y;
    int32_t trackH = m_rect.h;
    r->DrawRect({ 30, 30, 35, 200 }, trackX, trackY, kScrollbarW, trackH, SDL_BLENDMODE_BLEND);

    // 스크롤바 썸
    int32_t thumbH = std::max(kScrollbarMinH,
        static_cast<int32_t>(static_cast<float>(m_rect.h) / m_contentH * trackH));
    int32_t maxScroll = m_contentH - m_rect.h;
    float   ratio     = (maxScroll > 0) ? static_cast<float>(m_scrollY) / maxScroll : 0.0f;
    int32_t thumbY    = trackY + static_cast<int32_t>(ratio * (trackH - thumbH));

    r->DrawRect({ 110, 110, 120, 220 }, trackX, thumbY, kScrollbarW, thumbH, SDL_BLENDMODE_BLEND);
}

void IEScrollView::BeginDraw()
{
    IERenderer* r = GetRenderer();
    if (r == nullptr)
        return;

    SDL_Rect clip = { m_rect.x, m_rect.y, m_rect.w - kScrollbarW, m_rect.h };
    SDL_SetRenderClipRect(r->GetSDLRenderer(), &clip);
}

void IEScrollView::EndDraw()
{
    IERenderer* r = GetRenderer();
    if (r == nullptr)
        return;

    SDL_SetRenderClipRect(r->GetSDLRenderer(), nullptr);
}
