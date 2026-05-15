#pragma once
#include "../InspirationEngine/InspirationEngine.h"

/// <summary>
/// 에디터 패널 콘텐츠 추상 인터페이스.
/// IEDockedPanel(메인 창 내)과 IEFloatingPanelWindow(독립 SDL 창) 양쪽에서 렌더링.
/// </summary>
class IEEditorPanel
{
public:
    virtual ~IEEditorPanel() = default;

    virtual void        Draw(IERenderer* r)                                              = 0;
    virtual void        Update(float dt)                                                 = 0;
    virtual const char* GetTitle()                                             const = 0;

    /// <summary>
    /// 패널 콘텐츠 영역의 절대 창 좌표. 컨테이너가 레이아웃 변경 시 호출.
    /// </summary>
    virtual void SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h)             = 0;

    virtual void SetFont(IEFont* f)        {}
    virtual void SetOwnerWindow(IEWindow* w) {}
    virtual void SetRenderer(IERenderer* r) {}
};
