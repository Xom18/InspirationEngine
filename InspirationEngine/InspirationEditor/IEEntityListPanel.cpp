#include "IEEntityListPanel.h"
#include <cstdio>

void IEEntityListPanel::SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;
}

void IEEntityListPanel::Update(float dt)
{
    if (m_scene == nullptr || m_ownerWindow == nullptr)
        return;
    if (IECore::GetMouseOnWindow() != m_ownerWindow)
        return;

    float gx = 0.0f, gy = 0.0f;
    SDL_MouseButtonFlags btn = SDL_GetGlobalMouseState(&gx, &gy);

    int32_t winX = 0, winY = 0;
    SDL_GetWindowPosition(m_ownerWindow->GetSDLWindow(), &winX, &winY);

    int32_t mx = static_cast<int32_t>(gx) - winX;
    int32_t my = static_cast<int32_t>(gy) - winY;

    bool lmb      = (btn & SDL_BUTTON_LMASK) != 0;
    bool lmbClick = lmb && !m_prevLMB;
    m_prevLMB     = lmb;

    if (lmbClick && mx >= m_x && mx < m_x + m_w && my >= m_y && my < m_y + m_h)
    {
        int32_t row = (my - m_y) / kRowH;
        int32_t objCount = static_cast<int32_t>(m_scene->GetObjects().size());
        m_selectedIndex = (row < objCount) ? row : -1;
    }
}

void IEEntityListPanel::Draw(IERenderer* r)
{
    if (r == nullptr)
        return;

    r->DrawRect(kColBg, m_x, m_y, m_w, m_h, SDL_BLENDMODE_NONE);

    if (m_scene == nullptr || m_font == nullptr)
        return;

    const auto& objs = m_scene->GetObjects();
    int32_t rowY = m_y;
    for (int32_t i = 0; i < static_cast<int32_t>(objs.size()); ++i)
    {
        if (rowY + kRowH > m_y + m_h)
            break;

        SDL_Color rowCol = (i == m_selectedIndex) ? kColSelRow : kColRow;
        if (i % 2 == 0)
            r->DrawRect(rowCol, m_x, rowY, m_w, kRowH, SDL_BLENDMODE_NONE);

        char buf[32];
        auto* t = objs[i]->GetComponent<IETransformComponent>();
        if (t != nullptr)
            std::snprintf(buf, sizeof(buf), "Object %d  (%.0f, %.0f)", i, t->GetX(), t->GetY());
        else
            std::snprintf(buf, sizeof(buf), "Object %d", i);

        r->DrawText(m_font, buf, kColText, m_x + 6, rowY + 4);
        rowY += kRowH;
    }
}
