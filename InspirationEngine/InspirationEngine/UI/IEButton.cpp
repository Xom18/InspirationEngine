#include "../InspirationEngine.h"

void IEButton::Draw()
{
    IERenderer* r = GetRenderer();
    if (r == nullptr)
        return;

    SDL_Rect rect = IEUIBase::GetRect();
    SDL_Color col = m_pressed ? m_colorPressed
                  : m_hovered ? m_colorHover
                              : m_colorNormal;

    r->DrawRect(col, rect.x, rect.y, rect.w, rect.h, SDL_BLENDMODE_NONE);

    IEFont* font = GetFont();
    if (font == nullptr || m_label.empty())
        return;

    int32_t fontH = font->GetHeight();
    int32_t textY = rect.y + (rect.h - fontH) / 2;
    r->DrawText(font, m_label.c_str(), m_colorText, rect.x + 6, textY);
}

void IEButton::Update()
{
    IEWindow* ownerWindow = GetOwnerWindow();
    if (ownerWindow == nullptr || IECore::GetMouseOnWindow() != ownerWindow)
    {
        m_hovered = false;
        m_pressed = false;
        m_prevLMB = false;
        return;
    }

    float gx = 0.0f, gy = 0.0f;
    SDL_MouseButtonFlags btn = SDL_GetGlobalMouseState(&gx, &gy);

    int32_t winX = 0, winY = 0;
    SDL_GetWindowPosition(ownerWindow->GetSDLWindow(), &winX, &winY);

    int32_t mx = static_cast<int32_t>(gx) - winX;
    int32_t my = static_cast<int32_t>(gy) - winY;

    bool lmb     = (btn & SDL_BUTTON_LMASK) != 0;
    bool clicked = lmb && !m_prevLMB;

    SDL_Rect rect = IEUIBase::GetRect();
    m_hovered = (mx >= rect.x && mx < rect.x + rect.w &&
                 my >= rect.y && my < rect.y + rect.h);

    m_pressed = lmb && m_hovered;

    if (clicked && m_hovered && m_callback)
        m_callback();

    m_prevLMB = lmb;
}
