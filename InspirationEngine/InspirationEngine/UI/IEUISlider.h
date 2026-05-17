#pragma once
#include <functional>

class IEUISlider : public IEUIBase
{
public:
    virtual void Draw() override;
    virtual void Update() override;

    /// <summary>
    /// 현재 값 옆에 숫자 표시 여부
    /// </summary>
    void SetShowValue(bool show) { m_showValue = show; }

    /// <summary>
    /// 슬라이더 값 범위 설정
    /// </summary>
    void  SetRange(float minVal, float maxVal);

    /// <summary>
    /// 슬라이더 값 설정 (범위 클램프)
    /// </summary>
    void  SetValue(float v);

    /// <summary>
    /// 현재 슬라이더 값 반환
    /// </summary>
    float GetValue() const { return m_value; }

    /// <summary>
    /// 값 변경 시 호출될 콜백 설정
    /// </summary>
    void SetCallback(std::function<void(float)> callback) { m_callback = std::move(callback); }

private:
    float ThumbCenterX() const;
    float ValueFromX(int32_t x) const;

    static constexpr int32_t kTrackH = 4;
    static constexpr int32_t kThumbW = 12;
    static constexpr int32_t kThumbH = 20;

    float m_minVal     = 0.0f;
    float m_maxVal     = 1.0f;
    float m_value      = 0.0f;
    bool  m_isDragging = false;
    bool  m_prevLMB    = false;
    bool  m_showValue  = true;
    std::function<void(float)> m_callback;
};
