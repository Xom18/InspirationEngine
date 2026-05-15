#pragma once
#include "IEEditorPanel.h"
#include <memory>

/// <summary>
/// 메인 창 안에 배치되는 패널 컨테이너.
/// 타이틀바 + 드래그 이동 + 창 밖 드래그 시 undock 신호.
/// </summary>
class IEDockedPanel
{
public:
    IEDockedPanel() = default;
    explicit IEDockedPanel(std::unique_ptr<IEEditorPanel> panel);

    IEDockedPanel(IEDockedPanel&&)            = default;
    IEDockedPanel& operator=(IEDockedPanel&&) = default;
    IEDockedPanel(const IEDockedPanel&)       = delete;
    IEDockedPanel& operator=(const IEDockedPanel&) = delete;

    void SetOwnerWindow(IEWindow* win);
    void SetFont(IEFont* f);
    void SetRenderer(IERenderer* r);
    void SetRect(int32_t x, int32_t y, int32_t w, int32_t h);

    void Draw();
    void Update(float dt);

    bool ShouldUndock()        const { return m_shouldUndock; }
    void ClearUndockFlag()           { m_shouldUndock = false; }

    int32_t GetUndockScreenX() const { return m_undockScreenX; }
    int32_t GetUndockScreenY() const { return m_undockScreenY; }
    int32_t GetUndockW()       const { return m_rect.w; }
    int32_t GetUndockH()       const { return m_rect.h; }

    /// <summary>
    /// undock 시 패널 소유권을 부동 창으로 이전
    /// </summary>
    std::unique_ptr<IEEditorPanel> ReleasePanel();

    SDL_Rect GetRect()  const { return m_rect; }
    bool     IsAlive()  const { return m_panel != nullptr; }

    /// <summary>
    /// LMB가 타이틀바 위에서 눌렸는지 (클릭한 프레임만 true → z-order 갱신용)
    /// </summary>
    bool WasTitleClicked() const { return m_titleClickedThisFrame; }

private:
    static constexpr int32_t  kTitleH      = 24;
    static constexpr SDL_Color kColTitleBg  = {  55,  55,  62, 255 };
    static constexpr SDL_Color kColTitleTxt = { 200, 200, 200, 255 };
    static constexpr SDL_Color kColBorder   = {  75,  75,  82, 255 };
    static constexpr SDL_Color kColContent  = {  35,  35,  40, 255 };

    void PropagateContentRect();

    std::unique_ptr<IEEditorPanel> m_panel;
    IEWindow*   m_ownerWindow = nullptr;
    IEFont*     m_font        = nullptr;
    IERenderer* m_renderer    = nullptr;

    SDL_Rect m_rect = {};

    bool    m_dragging     = false;
    int32_t m_dragOffsetX  = 0;
    int32_t m_dragOffsetY  = 0;
    bool    m_prevLMB      = false;

    bool    m_titleClickedThisFrame = false;
    bool    m_shouldUndock  = false;
    int32_t m_undockScreenX = 0;
    int32_t m_undockScreenY = 0;
};
