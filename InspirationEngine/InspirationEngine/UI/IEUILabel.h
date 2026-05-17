#pragma once
#include <string>

class IEUILabel : public IEUIBase
{
public:
    virtual void Draw() override;

    /// <summary>
    /// 위젯 영역 설정 — x, y만 사용, w/h 무시
    /// </summary>
    virtual void SetRect(int32_t x, int32_t y, int32_t /*w*/, int32_t /*h*/) override
    {
        IEUIBase::SetRect(x, y, 0, 0);
    }

    /// <summary>
    /// 위치 설정
    /// </summary>
    void SetPos(int32_t x, int32_t y) { IEUIBase::SetRect(x, y, 0, 0); }

    /// <summary>
    /// 표시할 텍스트 설정 (UTF-8)
    /// </summary>
    void SetText(const char* text) { m_text = text; }

    /// <summary>
    /// 텍스트 색상 설정
    /// </summary>
    void SetColor(SDL_Color color) { m_color = color; }

private:
    std::string m_text;
    SDL_Color   m_color = { 220, 220, 220, 255 };
};
