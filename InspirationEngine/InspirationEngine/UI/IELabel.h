#pragma once
#include <string>

class IELabel : public IEUIBase
{
public:
    virtual void Draw() override;

    /// <summary>
    /// 렌더링에 사용할 폰트 설정
    /// </summary>
    void SetFont(IEFont* font)
    {
        m_font = font;
    }

    /// <summary>
    /// 표시할 텍스트 설정 (UTF-8)
    /// </summary>
    void SetText(const char* text)
    {
        m_text = text;
    }

    /// <summary>
    /// 위치 설정
    /// </summary>
    void SetPos(int32_t x, int32_t y)
    {
        m_x = x;
        m_y = y;
    }

    /// <summary>
    /// 텍스트 색상 설정
    /// </summary>
    void SetColor(SDL_Color color)
    {
        m_color = color;
    }

private:
    IEFont*     m_font  = nullptr;
    std::string m_text;
    int32_t     m_x     = 0;
    int32_t     m_y     = 0;
    SDL_Color   m_color = { 220, 220, 220, 255 };
};
