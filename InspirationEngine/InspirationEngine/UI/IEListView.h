#pragma once
#include <functional>
#include <vector>
#include <string>

/// <summary>
/// 스크롤 가능한 단일 선택 목록 위젯.
/// </summary>
class IEListView : public IEUIBase
{
public:
    virtual void Draw()   override;
    virtual void Update() override;

    virtual void SetRect(int32_t x, int32_t y, int32_t w, int32_t h) override;
    virtual void SetOwnerWindow(IEWindow* w)  override;
    virtual void SetRenderer(IERenderer* r)   override;

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
    /// 항목 내용을 교체하되 스크롤·선택 상태는 유지.
    /// 항목 수 감소로 선택이 범위 밖이 되면 -1로 초기화.
    /// </summary>
    void RefreshItems(std::vector<std::string> items);

    /// <summary>
    /// 선택 인덱스 프로그래밍 방식으로 설정.
    /// </summary>
    void SetSelectedIndex(int32_t index);

    int32_t            GetSelectedIndex() const { return m_selectedIndex; }
    const std::string* GetSelectedItem()  const;

    /// <summary>
    /// 선택 변경 시 콜백. 인자: 새 선택 인덱스 (-1 = 없음).
    /// </summary>
    void SetCallback(std::function<void(int32_t)> cb) { m_callback = std::move(cb); }

private:
    void UpdateScrollHeight();

    static constexpr int32_t  kRowH      = 22;
    static constexpr SDL_Color kColBg     = {  35,  35,  40, 255 };
    static constexpr SDL_Color kColRow    = {  45,  45,  52, 255 };
    static constexpr SDL_Color kColRowAlt = {  40,  40,  47, 255 };
    static constexpr SDL_Color kColSel    = {  60,  90, 140, 255 };
    static constexpr SDL_Color kColHover  = {  55,  65,  80, 255 };
    static constexpr SDL_Color kColText   = { 200, 200, 200, 255 };

    std::vector<std::string>     m_items;
    int32_t                      m_selectedIndex = -1;
    int32_t                      m_hoveredIndex  = -1;
    bool                         m_prevLMB       = false;
    std::function<void(int32_t)> m_callback;

    IEScrollView m_scroll;
};
