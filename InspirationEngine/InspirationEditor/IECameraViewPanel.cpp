#include "IECameraViewPanel.h"

void IECameraViewPanel::SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    m_x = x;
    m_y = y;
    m_w = (w > 0) ? w : 1;
    m_h = (h > 0) ? h : 1;
}

void IECameraViewPanel::Draw(IERenderer* r)
{
    if (r == nullptr)
        return;

    SDL_Renderer* sdlR = r->GetSDLRenderer();
    SDL_Rect vpRect = { m_x, m_y, m_w, m_h };
    SDL_SetRenderViewport(sdlR, &vpRect);

    r->DrawRect(kColBg, 0, 0, m_w, m_h, SDL_BLENDMODE_NONE);

    if (m_scene != nullptr)
    {
        m_scene->SetViewportOverride(m_w, m_h);
        m_scene->Draw(r);
    }
    else
    {
        IEFont* font = GetFont();
        if (font != nullptr)
            r->DrawText(font, "No Camera", kColText, 8, 8);
    }

    SDL_SetRenderViewport(sdlR, nullptr);
}
