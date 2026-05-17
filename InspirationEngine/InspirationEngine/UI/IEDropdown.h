#pragma once
#include <functional>
#include <vector>
#include <string>

/// <summary>
/// 접힌/펼친 상태를 토글하는 드롭다운 선택 위젯.
/// 펼쳐진 목록은 위젯 아래쪽으로 렌더됨 — 다른 위젯보다 마지막에 그려야 함.
/// </summary>
class IEDropdown : public IEUIBase
{
public:
    virtual void Draw()   override;
    virtual void Update() override;

    virtual void SetRect(int32_t x, int32_t y, int32_t w, int32_t h) override;

    /// <summary>
    /// 항목 목록을 교체하고 선택을 초기화.
    /// </summary>
    void SetItems(std::vector<std::string> items);

    /// <summary>
    /// 항목 하나 추가.
    /// </summary>
    void AddItem(const std::string& item);

    /// <summary>
    /// 모든 항목 제거.
    /// </summary>
    void ClearItems();

    /// <summary>
    /// 선택 인덱스 프로그래밍 방식으로 설정.
    /// </summary>
    void SetSelectedIndex(int32_t index);

    int32_t            GetSelectedIndex() const { return m_selectedIndex; }
    const std::string* GetSelectedItem()  const;
    bool               IsOpen()           const { return m_isOpen; }

    /// <summary>
    /// 드롭다운을 강제로 닫음.
    /// </summary>
    void Close() { m_isOpen = false; }

    /// <summary>
    /// 선택 변경 시 콜백. 인자: 새 선택 인덱스.
    /// </summary>
    void SetCallback(std::function<void(int32_t)> cb) { m_callback = std::move(cb); }

    /// <summary>
    /// 열린 팝업 목록을 최상위에 재렌더 (z-order 보정용).
    /// IESection::DrawOverlay 체인에서 자동 호출됨.
    /// </summary>
    void DrawOverlay() override;

private:
    void UpdateListRect();
    void DrawArrow(IERenderer* r, int32_t cx, int32_t cy) const;

    static constexpr int32_t  kArrowW      = 20;
    static constexpr int32_t  kRowH        = 22;
    static constexpr int32_t  kMaxVisible  = 6;
    static constexpr SDL_Color kColBg      = {  45,  45,  52, 255 };
    static constexpr SDL_Color kColBorder  = {  80,  80,  90, 255 };
    static constexpr SDL_Color kColListBg  = {  35,  35,  40, 255 };
    static constexpr SDL_Color kColRow     = {  40,  40,  47, 255 };
    static constexpr SDL_Color kColSel     = {  60,  90, 140, 255 };
    static constexpr SDL_Color kColHover   = {  55,  65,  80, 255 };
    static constexpr SDL_Color kColText    = { 200, 200, 200, 255 };
    static constexpr SDL_Color kColArrow   = { 160, 160, 160, 255 };

    std::vector<std::string>     m_items;
    int32_t                      m_selectedIndex = -1;
    int32_t                      m_hoveredIndex  = -1;
    bool                         m_isOpen        = false;
    bool                         m_prevLMB       = false;
    int32_t                      m_scrollOffset  = 0;
    std::function<void(int32_t)> m_callback;

    SDL_Rect m_listRect = {};
};
