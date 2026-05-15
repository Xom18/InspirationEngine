#include "../InspirationEngine.h"
#include <algorithm>
#include <cstdio>

void IESlider::SetRange(float minVal, float maxVal)
{
    m_minVal = minVal;
    m_maxVal = maxVal;
    SetValue(m_value);
}

void IESlider::SetValue(float v)
{
    m_value = std::clamp(v, m_minVal, m_maxVal);
}

float IESlider::ThumbCenterX() const
{
    float t = (m_maxVal > m_minVal)
        ? (m_value - m_minVal) / (m_maxVal - m_minVal)
        : 0.0f;
    return static_cast<float>(m_rect.x) + t * static_cast<float>(m_rect.w);
}

float IESlider::ValueFromX(int32_t x) const
{
    float t = static_cast<float>(x - m_rect.x) / static_cast<float>(m_rect.w);
    t = std::clamp(t, 0.0f, 1.0f);
    return m_minVal + t * (m_maxVal - m_minVal);
}

void IESlider::Update()
{
    if (m_ownerWindow == nullptr || IECore::GetMouseOnWindow() != m_ownerWindow)
    {
        m_prevLMB    = false;
        m_isDragging = false;
        return;
    }

    float gx = 0.0f, gy = 0.0f;
    SDL_MouseButtonFlags btn = SDL_GetGlobalMouseState(&gx, &gy);

    int32_t winX = 0, winY = 0;
    SDL_GetWindowPosition(m_ownerWindow->GetSDLWindow(), &winX, &winY);

    int32_t mx = static_cast<int32_t>(gx) - winX;
    int32_t my = static_cast<int32_t>(gy) - winY;

    bool lmb = (btn & SDL_BUTTON_LMASK) != 0;

    if (!lmb)
    {
        m_isDragging = false;
        m_prevLMB    = false;
        return;
    }

    bool clicked = lmb && !m_prevLMB;
    m_prevLMB = lmb;

    if (clicked)
    {
        if (mx >= m_rect.x && mx < m_rect.x + m_rect.w &&
            my >= m_rect.y && my < m_rect.y + m_rect.h)
            m_isDragging = true;
    }

    if (m_isDragging)
    {
        float newVal = ValueFromX(mx);
        if (newVal != m_value)
        {
            m_value = newVal;
            if (m_callback)
                m_callback(m_value);
        }
    }
}

void IESlider::Draw()
{
    IERenderer* r = GetRenderer();
    if (r == nullptr)
        return;

    int32_t trackY  = m_rect.y + (m_rect.h - kTrackH) / 2;
    int32_t thumbCx = static_cast<int32_t>(ThumbCenterX());
    int32_t thumbX  = thumbCx - kThumbW / 2;
    int32_t thumbY  = m_rect.y + (m_rect.h - kThumbH) / 2;

    r->DrawRect({ 60, 60, 60, 255 }, m_rect.x, trackY, m_rect.w, kTrackH);

    int32_t fillW = thumbCx - m_rect.x;
    if (fillW > 0)
        r->DrawRect({ 100, 160, 220, 255 }, m_rect.x, trackY, fillW, kTrackH);

    SDL_Color thumbCol = m_isDragging
        ? SDL_Color{ 255, 255, 255, 255 }
        : SDL_Color{ 200, 200, 200, 255 };
    r->DrawRect(thumbCol, thumbX, thumbY, kThumbW, kThumbH);

    if (m_showValue && m_font != nullptr)
    {
        char buf[16];
        std::snprintf(buf, sizeof(buf), "%.2f", m_value);
        int32_t textY = m_rect.y + (m_rect.h - m_font->GetHeight()) / 2;
        r->DrawText(m_font, buf, { 200, 200, 200, 255 }, m_rect.x + m_rect.w + 6, textY);
    }
}
