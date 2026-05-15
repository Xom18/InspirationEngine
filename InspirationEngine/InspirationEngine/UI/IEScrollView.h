#pragma once

class IEWindow;

class IEScrollView : public IEUIBase
{
public:
    virtual void Draw() override;
    virtual void Update() override;

    /// <summary>
    /// 뷰포트 영역 설정
    /// </summary>
    void SetRect(int32_t x, int32_t y, int32_t w, int32_t h)
    {
        m_rect = { x, y, w, h };
    }

    /// <summary>
    /// 스크롤 가능한 전체 콘텐츠 높이 설정
    /// </summary>
    void SetContentHeight(int32_t contentH)
    {
        m_contentH = contentH;
        ClampScroll();
    }

    /// <summary>
    /// 마우스 이벤트를 소유한 창 설정
    /// </summary>
    void SetOwnerWindow(IEWindow* window)
    {
        m_ownerWindow = window;
    }

    /// <summary>
    /// 현재 스크롤 Y 오프셋 반환 — 콘텐츠 렌더 시 y 좌표에서 빼서 사용
    /// </summary>
    int32_t GetScrollOffsetY() const
    {
        return m_scrollY;
    }

    /// <summary>
    /// 스크롤을 맨 위로 초기화
    /// </summary>
    void ResetScroll()
    {
        m_scrollY = 0;
    }

    /// <summary>
    /// 콘텐츠 렌더 전 호출 — 뷰포트 영역으로 SDL 클립 설정
    /// </summary>
    void BeginDraw();

    /// <summary>
    /// 콘텐츠 렌더 후 호출 — SDL 클립 해제
    /// </summary>
    void EndDraw();

private:
    void ClampScroll();

    static constexpr int32_t kScrollbarW    = 8;
    static constexpr int32_t kScrollbarMinH = 16;
    static constexpr float   kScrollSpeed   = 40.0f;

    IEWindow* m_ownerWindow = nullptr;
    SDL_Rect  m_rect        = {};
    int32_t   m_contentH    = 0;
    int32_t   m_scrollY     = 0;
};
