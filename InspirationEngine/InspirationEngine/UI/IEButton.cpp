#include "../InspirationEngine.h"

void IEButton::Draw()
{
    IERenderer* r = GetRenderer();
    if (r == nullptr)
        return;

    SDL_Color col = m_pressed ? m_colorPressed
                  : m_hovered ? m_colorHover
                              : m_colorNormal;

    r->DrawRect(col, m_rect.x, m_rect.y, m_rect.w, m_rect.h, SDL_BLENDMODE_NONE);

    if (m_font == nullptr || m_label.empty())
        return;

    int32_t fontH = m_font->GetHeight();
    int32_t textY = m_rect.y + (m_rect.h - fontH) / 2;
    r->DrawText(m_font, m_label.c_str(), m_colorText, m_rect.x + 6, textY);
}

void IEButton::Update()
{
    if (m_ownerWindow == nullptr || IECore::GetMouseOnWindow() != m_ownerWindow)
    {
        m_hovered  = false;
        m_pressed  = false;
        m_prevLMB  = false;
        return;
    }

    float gx = 0.0f, gy = 0.0f;
    SDL_MouseButtonFlags btn = SDL_GetGlobalMouseState(&gx, &gy);

    int32_t winX = 0, winY = 0;
    SDL_GetWindowPosition(m_ownerWindow->GetSDLWindow(), &winX, &winY);

    int32_t mx = static_cast<int32_t>(gx) - winX;
    int32_t my = static_cast<int32_t>(gy) - winY;

    bool lmb     = (btn & SDL_BUTTON_LMASK) != 0;
    bool clicked = lmb && !m_prevLMB;

    m_hovered = (mx >= m_rect.x && mx < m_rect.x + m_rect.w &&
                 my >= m_rect.y && my < m_rect.y + m_rect.h);

    m_pressed = lmb && m_hovered;

    if (clicked && m_hovered && m_callback)
        m_callback();

    m_prevLMB = lmb;
}
