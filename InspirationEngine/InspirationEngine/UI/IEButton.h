#pragma once
#include <functional>
#include <string>

class IEButton : public IEUIBase
{
public:
    virtual void Draw() override;
    virtual void Update() override;

    /// <summary>
    /// 버튼 영역 반환
    /// </summary>
    void GetRect(SDL_Rect& rect) const { rect = IEUIBase::GetRect(); }

    /// <summary>
    /// 버튼 라벨 설정 (UTF-8)
    /// </summary>
    void SetLabel(const char* label) { m_label = label; }

    /// <summary>
    /// 클릭 콜백 설정
    /// </summary>
    void SetCallback(std::function<void()> callback) { m_callback = std::move(callback); }

    /// <summary>
    /// 일반 상태 배경색
    /// </summary>
    void SetColorNormal(SDL_Color color)  { m_colorNormal  = color; }

    /// <summary>
    /// 호버 상태 배경색
    /// </summary>
    void SetColorHover(SDL_Color color)   { m_colorHover   = color; }

    /// <summary>
    /// 눌린 상태 배경색
    /// </summary>
    void SetColorPressed(SDL_Color color) { m_colorPressed = color; }

    /// <summary>
    /// 라벨 텍스트 색상
    /// </summary>
    void SetTextColor(SDL_Color color)    { m_colorText    = color; }

    /// <summary>
    /// 현재 호버 상태 여부
    /// </summary>
    bool IsHovered() const { return m_hovered; }

    /// <summary>
    /// 현재 눌린 상태 여부
    /// </summary>
    bool IsPressed() const { return m_pressed; }

private:
    std::function<void()> m_callback;
    std::string           m_label;

    SDL_Color m_colorNormal  = {  70,  90, 120, 255 };
    SDL_Color m_colorHover   = {  90, 115, 150, 255 };
    SDL_Color m_colorPressed = {  50,  70, 100, 255 };
    SDL_Color m_colorText    = { 220, 220, 220, 255 };

    bool m_hovered = false;
    bool m_pressed = false;
    bool m_prevLMB = false;
};
