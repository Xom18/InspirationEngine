#pragma once

class IERenderer;
class IEFont;
class IEWindow;

/// <summary>
/// 패널 콘텐츠 추상 인터페이스.
/// IEDockedPanel(창 안 도킹) 및 IEFloatingPanel(독립 창) 양쪽에서 사용.
/// </summary>
class IEPanel
{
public:
    virtual ~IEPanel() = default;

    virtual void        Draw(IERenderer* r)                                      = 0;
    virtual void        Update(float dt)                                         = 0;
    virtual const char* GetTitle()                                     const = 0;

    /// <summary>
    /// 패널 콘텐츠 영역의 절대 창 좌표. 컨테이너가 레이아웃 변경 시 호출.
    /// </summary>
    virtual void        SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h) = 0;

    virtual void SetFont(IEFont* f)          {}
    virtual void SetOwnerWindow(IEWindow* w)  {}
    virtual void SetRenderer(IERenderer* r)   {}
};
