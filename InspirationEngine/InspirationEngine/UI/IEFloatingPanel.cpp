#include "../InspirationEngine.h"
#include "IEFloatingPanel.h"

void IEFloatingPanel::Init(std::unique_ptr<IEPanel> panel, IEFont* font, const std::string& windowId)
{
    m_panel    = std::move(panel);
    m_font     = font;
    m_windowId = windowId;

    if (m_panel != nullptr)
    {
        m_panel->SetFont(m_font);
        m_panel->SetOwnerWindow(this);
        m_panel->SetRenderer(GetRenderer(0));
        int32_t cH = std::max(0, GetHeight() - kDragBarH);
        m_panel->SetContentRect(0, kDragBarH, GetWidth(), cH);
    }
}

void IEFloatingPanel::OnResize(int32_t w, int32_t h)
{
    if (m_panel != nullptr)
    {
        int32_t cH = std::max(0, h - kDragBarH);
        m_panel->SetContentRect(0, kDragBarH, w, cH);
    }
}

void IEFloatingPanel::Update(float dt)
{
    if (m_panel != nullptr)
        m_panel->Update(dt);

    SDL_Window* sdlWin = GetSDLWindow();
    if (sdlWin == nullptr)
        return;

    float gxF = 0.0f, gyF = 0.0f;
    SDL_MouseButtonFlags btn = SDL_GetGlobalMouseState(&gxF, &gyF);
    bool lmb      = (btn & SDL_BUTTON_LMASK) != 0;
    bool lmbClick = lmb && !m_prevLMB;
    m_prevLMB     = lmb;

    int32_t gx = static_cast<int32_t>(gxF);
    int32_t gy = static_cast<int32_t>(gyF);

    if (lmbClick && IECore::GetMouseOnWindow() == this)
    {
        int32_t wx = 0, wy = 0;
        SDL_GetWindowPosition(sdlWin, &wx, &wy);
        int32_t mx = gx - wx;
        int32_t my = gy - wy;

        bool onClose = (mx >= GetWidth() - kCloseW && mx < GetWidth() &&
                        my >= 0 && my < kDragBarH);
        if (onClose)
        {
            IECore::RequestRemoveWindow(m_windowId.c_str());
            return;
        }

        bool onDrag = (mx >= 0 && mx < GetWidth() - kCloseW &&
                       my >= 0 && my < kDragBarH);
        if (onDrag)
        {
            m_dragging    = true;
            m_dragOffsetX = gx - wx;
            m_dragOffsetY = gy - wy;
        }
    }

    if (!lmb)
        m_dragging = false;

    if (m_dragging)
    {
        int32_t wx = 0, wy = 0;
        SDL_GetWindowPosition(sdlWin, &wx, &wy);
        SDL_SetWindowPosition(sdlWin, gx - m_dragOffsetX, gy - m_dragOffsetY);
    }
}

void IEFloatingPanel::Draw()
{
    IERenderer* r = GetRenderer(0);
    if (r == nullptr)
        return;

    r->DrawRect(kColDragBar, 0, 0, GetWidth(), kDragBarH, SDL_BLENDMODE_NONE);

    if (m_font != nullptr && m_panel != nullptr && m_panel->GetTitle() != nullptr)
        r->DrawText(m_font, m_panel->GetTitle(), kColDragText, 6, 5);

    r->DrawRect(kColClose, GetWidth() - kCloseW, 0, kCloseW, kDragBarH, SDL_BLENDMODE_NONE);
    if (m_font != nullptr)
        r->DrawText(m_font, "X", kColDragText, GetWidth() - kCloseW + 8, 5);

    if (m_panel != nullptr)
        m_panel->Draw(r);
}

void IEFloatingPanel::CallXButton()
{
    IECore::RequestRemoveWindow(m_windowId.c_str());
}
