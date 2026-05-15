#include "IEDockedPanel.h"
#include "../InspirationEngine.h"

IEDockedPanel::IEDockedPanel(std::unique_ptr<IEPanel> panel)
    : m_panel(std::move(panel))
{}

void IEDockedPanel::SetOwnerWindow(IEWindow* win)
{
    m_ownerWindow = win;
    if (m_panel != nullptr)
        m_panel->SetOwnerWindow(win);
}

void IEDockedPanel::SetFont(IEFont* f)
{
    m_font = f;
    if (m_panel != nullptr)
        m_panel->SetFont(f);
}

void IEDockedPanel::SetRenderer(IERenderer* r)
{
    m_renderer = r;
    if (m_panel != nullptr)
        m_panel->SetRenderer(r);
}

void IEDockedPanel::SetRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    m_rect = { x, y, w, h };
    PropagateContentRect();
}

void IEDockedPanel::PropagateContentRect()
{
    if (m_panel == nullptr)
        return;
    int32_t contentH = m_rect.h - kTitleH;
    if (contentH < 0)
        contentH = 0;
    m_panel->SetContentRect(m_rect.x, m_rect.y + kTitleH, m_rect.w, contentH);
}

std::unique_ptr<IEPanel> IEDockedPanel::ReleasePanel()
{
    return std::move(m_panel);
}

void IEDockedPanel::Update(float dt)
{
    m_titleClickedThisFrame = false;

    if (m_panel == nullptr || m_ownerWindow == nullptr)
        return;

    m_panel->Update(dt);

    float gx = 0.0f, gy = 0.0f;
    SDL_MouseButtonFlags btnState = SDL_GetGlobalMouseState(&gx, &gy);

    int32_t winX = 0, winY = 0;
    SDL_GetWindowPosition(m_ownerWindow->GetSDLWindow(), &winX, &winY);

    int32_t mx = static_cast<int32_t>(gx) - winX;
    int32_t my = static_cast<int32_t>(gy) - winY;

    bool lmb      = (btnState & SDL_BUTTON_LMASK) != 0;
    bool lmbClick = lmb && !m_prevLMB;
    m_prevLMB     = lmb;

    bool mouseOnThisWindow = (IECore::GetMouseOnWindow() == m_ownerWindow);

    if (lmbClick && mouseOnThisWindow)
    {
        bool onTitle = (mx >= m_rect.x && mx < m_rect.x + m_rect.w &&
                        my >= m_rect.y && my < m_rect.y + kTitleH);
        if (onTitle)
        {
            m_titleClickedThisFrame = true;
            m_dragging    = true;
            m_dragOffsetX = mx - m_rect.x;
            m_dragOffsetY = my - m_rect.y;
        }
    }

    if (!lmb)
        m_dragging = false;

    if (m_dragging)
    {
        m_rect.x = mx - m_dragOffsetX;
        m_rect.y = my - m_dragOffsetY;
        PropagateContentRect();

        // 마우스가 창 밖으로 나가면 undock
        int32_t winW = m_ownerWindow->GetWidth();
        int32_t winH = m_ownerWindow->GetHeight();
        int32_t absX = static_cast<int32_t>(gx);
        int32_t absY = static_cast<int32_t>(gy);
        bool outside = (absX < winX || absX > winX + winW ||
                        absY < winY || absY > winY + winH);
        if (outside)
        {
            m_shouldUndock  = true;
            m_undockScreenX = winX + m_rect.x;
            m_undockScreenY = winY + m_rect.y;
            m_dragging      = false;
        }
    }
}

void IEDockedPanel::Draw()
{
    if (m_panel == nullptr || m_renderer == nullptr)
        return;

    // 타이틀바
    m_renderer->DrawRect(kColTitleBg, m_rect.x, m_rect.y, m_rect.w, kTitleH, SDL_BLENDMODE_NONE);
    if (m_font != nullptr)
        m_renderer->DrawText(m_font, m_panel->GetTitle(), kColTitleTxt, m_rect.x + 6, m_rect.y + 5);

    // 테두리
    m_renderer->DrawLine(kColBorder, m_rect.x,             m_rect.y,            m_rect.x + m_rect.w, m_rect.y);
    m_renderer->DrawLine(kColBorder, m_rect.x,             m_rect.y + m_rect.h, m_rect.x + m_rect.w, m_rect.y + m_rect.h);
    m_renderer->DrawLine(kColBorder, m_rect.x,             m_rect.y,            m_rect.x,             m_rect.y + m_rect.h);
    m_renderer->DrawLine(kColBorder, m_rect.x + m_rect.w, m_rect.y,            m_rect.x + m_rect.w,  m_rect.y + m_rect.h);

    // 콘텐츠 배경
    int32_t contentH = m_rect.h - kTitleH;
    if (contentH > 0)
        m_renderer->DrawRect(kColContent, m_rect.x, m_rect.y + kTitleH, m_rect.w, contentH, SDL_BLENDMODE_NONE);

    m_panel->Draw(m_renderer);
}
