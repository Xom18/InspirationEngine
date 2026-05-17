#include "../InspirationEngine.h"

void IEUIScrollView::ClampScroll()
{
    SDL_Rect rect = GetRect();
    int32_t maxScroll = m_contentH - rect.h;
    if (maxScroll < 0)
        maxScroll = 0;
    if (m_scrollY < 0)
        m_scrollY = 0;
    if (m_scrollY > maxScroll)
        m_scrollY = maxScroll;
}

void IEUIScrollView::Update()
{
    IEWindow* ownerWindow = GetOwnerWindow();
    if (ownerWindow == nullptr || IECore::GetMouseOnWindow() != ownerWindow)
        return;

    float gx = 0.0f, gy = 0.0f;
    SDL_GetGlobalMouseState(&gx, &gy);

    int32_t winX = 0, winY = 0;
    SDL_GetWindowPosition(ownerWindow->GetSDLWindow(), &winX, &winY);

    int32_t mx = static_cast<int32_t>(gx) - winX;
    int32_t my = static_cast<int32_t>(gy) - winY;

    SDL_Rect rect = GetRect();
    bool onViewport = (mx >= rect.x && mx < rect.x + rect.w &&
                       my >= rect.y && my < rect.y + rect.h);
    if (!onViewport)
        return;

    float wheel = IECore::GetInput().GetMouseWheelY();
    if (wheel != 0.0f)
    {
        m_scrollY -= static_cast<int32_t>(wheel * kScrollSpeed);
        ClampScroll();
    }
}

void IEUIScrollView::Draw()
{
    IERenderer* r = GetRenderer();
    SDL_Rect rect = GetRect();
    if (r == nullptr || m_contentH <= rect.h)
        return;

    int32_t trackX = rect.x + rect.w - kScrollbarW;
    int32_t trackY = rect.y;
    int32_t trackH = rect.h;
    r->DrawRect({ 30, 30, 35, 200 }, trackX, trackY, kScrollbarW, trackH, SDL_BLENDMODE_BLEND);

    int32_t thumbH = std::max(kScrollbarMinH,
        static_cast<int32_t>(static_cast<float>(rect.h) / m_contentH * trackH));
    int32_t maxScroll = m_contentH - rect.h;
    float   ratio     = (maxScroll > 0) ? static_cast<float>(m_scrollY) / maxScroll : 0.0f;
    int32_t thumbY    = trackY + static_cast<int32_t>(ratio * (trackH - thumbH));

    r->DrawRect({ 110, 110, 120, 220 }, trackX, thumbY, kScrollbarW, thumbH, SDL_BLENDMODE_BLEND);
}

void IEUIScrollView::BeginDraw()
{
    IERenderer* r = GetRenderer();
    if (r == nullptr)
        return;

    SDL_Rect rect = GetRect();
    SDL_Rect clip = { rect.x, rect.y, rect.w - kScrollbarW, rect.h };
    SDL_SetRenderClipRect(r->GetSDLRenderer(), &clip);
}

void IEUIScrollView::EndDraw()
{
    IERenderer* r = GetRenderer();
    if (r == nullptr)
        return;

    SDL_SetRenderClipRect(r->GetSDLRenderer(), nullptr);
}
