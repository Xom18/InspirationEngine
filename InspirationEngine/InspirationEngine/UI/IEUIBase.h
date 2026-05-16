#pragma once
#include <SDL3/SDL.h>

class IERenderer;
class IEFont;
class IEWindow;

class IEUIBase
{
public:
    IEUIBase() = default;
    virtual ~IEUIBase() = default;

    /// <summary>
    /// 렌더러 설정
    /// </summary>
    virtual void SetRenderer(IERenderer* renderer)                              { m_renderer    = renderer; }

    /// <summary>
    /// 렌더링에 사용할 폰트 설정
    /// </summary>
    virtual void SetFont(IEFont* font)                                          { m_font        = font; }

    /// <summary>
    /// 마우스 이벤트를 소유한 창 설정
    /// </summary>
    virtual void SetOwnerWindow(IEWindow* win)                                  { m_ownerWindow = win; }

    /// <summary>
    /// 위젯 영역 설정
    /// </summary>
    virtual void SetRect(int32_t x, int32_t y, int32_t w, int32_t h)          { m_rect        = { x, y, w, h }; }

    virtual void Draw()   {}
    virtual void Update() {}

protected:
    IERenderer* GetRenderer()    const { return m_renderer; }
    IEFont*     GetFont()        const { return m_font; }
    IEWindow*   GetOwnerWindow() const { return m_ownerWindow; }
    SDL_Rect    GetRect()        const { return m_rect; }

private:
    IERenderer* m_renderer    = nullptr;
    IEFont*     m_font        = nullptr;
    IEWindow*   m_ownerWindow = nullptr;
    SDL_Rect    m_rect        = {};
};
