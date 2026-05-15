#pragma once
#include <functional>

class IEWindow;
class IEFont;

class IESlider : public IEUIBase
{
public:
    virtual void Draw() override;
    virtual void Update() override;

    void SetRect(int32_t x, int32_t y, int32_t w, int32_t h) { m_rect = { x, y, w, h }; }
    void SetFont(IEFont* font)         { m_font = font; }
    void SetOwnerWindow(IEWindow* win) { m_ownerWindow = win; }
    void SetShowValue(bool show)       { m_showValue = show; }

    void  SetRange(float minVal, float maxVal);
    void  SetValue(float v);
    float GetValue() const { return m_value; }

    void SetCallback(std::function<void(float)> callback)
    {
        m_callback = std::move(callback);
    }

private:
    float ThumbCenterX() const;
    float ValueFromX(int32_t x) const;

    static constexpr int32_t kTrackH = 4;
    static constexpr int32_t kThumbW = 12;
    static constexpr int32_t kThumbH = 20;

    IEFont*    m_font        = nullptr;
    IEWindow*  m_ownerWindow = nullptr;
    SDL_Rect   m_rect        = {};
    float      m_minVal      = 0.0f;
    float      m_maxVal      = 1.0f;
    float      m_value       = 0.0f;
    bool       m_isDragging  = false;
    bool       m_prevLMB     = false;
    bool       m_showValue   = true;
    std::function<void(float)> m_callback;
};
