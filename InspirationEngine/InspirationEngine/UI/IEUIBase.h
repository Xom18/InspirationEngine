#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <memory>

class IERenderer;
class IEFont;
class IEWindow;

class IEUIBase
{
public:
    IEUIBase() = default;
    virtual ~IEUIBase() = default;

    /// <summary>렌더러 설정 — 자식에 자동 전파</summary>
    virtual void SetRenderer(IERenderer* renderer);

    /// <summary>폰트 설정 — 자식에 자동 전파</summary>
    virtual void SetFont(IEFont* font);

    /// <summary>소유 창 설정 — 자식에 자동 전파</summary>
    virtual void SetOwnerWindow(IEWindow* win);

    /// <summary>위젯 영역 설정</summary>
    virtual void SetRect(int32_t x, int32_t y, int32_t w, int32_t h) { m_rect = { x, y, w, h }; }

    virtual void Draw()   {}
    virtual void Update() {}

    /// <summary>
    /// z-order 후처리용 오버레이 렌더. Dropdown 팝업 등 최상위 렌더가 필요한 위젯이 오버라이드.
    /// 기본: 자식에 전파.
    /// </summary>
    virtual void DrawOverlay();

    /// <summary>
    /// 절대 좌표 기준 레이아웃 계산. 반환값 = 이 위젯이 소비한 픽셀 높이.
    /// 기본: 0 반환 (단순 위젯).
    /// </summary>
    virtual int32_t Layout(int32_t x, int32_t y, int32_t w) { (void)x; (void)y; (void)w; return 0; }

    /// <summary>이 위젯이 레이블이 있는 행(IERow)인지 여부. IERow만 true 반환.</summary>
    virtual bool IsRow() const { return false; }

    /// <summary>행 짝수/홀수 여부 설정 (배경색 교대용). IERow만 구현.</summary>
    virtual void SetRowParity(bool /*even*/) {}

protected:
    /// <summary>자식 추가 — 컨텍스트(font/renderer/window) 자동 전파 후 소유권 이전.</summary>
    template<typename T>
    T* AddChild(std::unique_ptr<T> child)
    {
        child->SetFont(m_font);
        child->SetRenderer(m_renderer);
        child->SetOwnerWindow(m_ownerWindow);
        T* ptr = child.get();
        m_children.push_back(std::move(child));
        return ptr;
    }

    std::vector<std::unique_ptr<IEUIBase>> m_children;

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
