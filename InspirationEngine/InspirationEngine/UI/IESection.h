#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>

class IEFont;
class IEWindow;
class IERenderer;
class IETextBox;
class IELabel;
class IESlider;
class IEDropdown;

/// <summary>
/// 재귀 가능한 UI 섹션 컨테이너.
/// AddSection / AddTextBox / AddSlider / AddLabel / AddDropdown 로 자식 추가.
/// Layout(x, y, w) 후 Draw / Update 사용.
/// 게임 UI 및 에디터 공용.
/// </summary>
class IESection
{
public:
    static constexpr int32_t kHeaderH  = 22;
    static constexpr int32_t kRowH     = 24;
    static constexpr int32_t kPadY     = 2;
    static constexpr int32_t kIndentX  = 8;
    static constexpr int32_t kLabelW   = 72;
    static constexpr int32_t kMarginR  = 4;

    void SetTitle(const std::string& title) { m_title = title; }
    const std::string& GetTitle() const     { return m_title; }

    void SetCollapsible(bool c) { m_collapsible = c; }
    void SetCollapsed(bool c)   { m_collapsed   = c; }
    bool IsCollapsed() const    { return m_collapsed; }

    /// <summary>타이틀바 오른쪽(45% 지점)에 표시할 보조 레이블.</summary>
    void SetHeaderLabel(const std::string& text)  { m_headerLabel = text; }
    void SetHeaderLabelColor(SDL_Color col)        { m_headerLabelColor = col; }

    // ── 자식 추가 (소유권은 이 섹션) ─────────────────────────────────

    /// <summary>자식 섹션 추가. 반환 포인터는 이 섹션이 소유.</summary>
    IESection*  AddSection(const std::string& title);

    /// <summary>텍스트박스 행 추가. rowLabel = 왼쪽 고정 레이블 텍스트.</summary>
    IETextBox*  AddTextBox(const std::string& rowLabel = "");

    /// <summary>슬라이더 행 추가.</summary>
    IESlider*   AddSlider(const std::string& rowLabel, float minV, float maxV);

    /// <summary>전체 너비 레이블 행 추가 (표시 전용).</summary>
    IELabel*    AddLabel(const std::string& text = "");

    /// <summary>드롭다운 행 추가. items = 선택 항목 목록.</summary>
    IEDropdown* AddDropdown(const std::string& rowLabel, std::vector<std::string> items);

    // ── 레이아웃 ──────────────────────────────────────────────────────

    /// <summary>
    /// 모든 자식의 절대 위치를 계산하고 내부 높이를 확정.
    /// 반환값 = 이 섹션이 차지한 총 높이(픽셀).
    /// </summary>
    int32_t Layout(int32_t x, int32_t y, int32_t w);

    /// <summary>마지막 Layout() 호출 이후 확정된 총 높이 반환</summary>
    int32_t GetTotalHeight() const { return m_totalH; }

    // ── 렌더링 / 입력 ────────────────────────────────────────────────

    void Draw(IERenderer* r);
    void Update();

    /// <summary>
    /// 열린 Dropdown 팝업을 재귀적으로 맨 위에 렌더링.
    /// 최상위 호출자(Panel::Draw)에서 모든 Draw() 호출 후 실행해야 함.
    /// </summary>
    void DrawOpenDropdowns(IERenderer* r);

    // ── 컨텍스트 전파 ────────────────────────────────────────────────

    void SetFont(IEFont* f);
    void SetOwnerWindow(IEWindow* w);
    void SetRenderer(IERenderer* r);

private:
    enum class EntryKind { TextBox, Slider, Label, Section, Dropdown };

    struct Entry
    {
        EntryKind              kind;
        std::string            rowLabel;

        std::unique_ptr<IETextBox>   textBox;
        std::unique_ptr<IESlider>    slider;
        std::unique_ptr<IELabel>     label;
        std::unique_ptr<IESection>   section;
        std::unique_ptr<IEDropdown>  dropdown;

        SDL_Rect  widgetRect{};  // Layout() 가 채움
        SDL_Rect  labelRect{};   // Layout() 가 채움
    };

    void PropagateToEntry(Entry& e);
    bool CheckHeaderClick();

    std::string        m_title;
    std::string        m_headerLabel;
    SDL_Color          m_headerLabelColor = { 140, 140, 150, 255 };  // kColLabel과 동일 초기값
    static constexpr float kHeaderLabelRatio = 0.45f;

    bool               m_collapsible = true;
    bool               m_collapsed   = false;
    std::vector<Entry> m_entries;

    // Layout 상태
    int32_t   m_x = 0, m_y = 0, m_w = 0;
    int32_t   m_totalH = kHeaderH;
    SDL_Rect  m_headerRect{};

    // 컨텍스트
    IEFont*     m_font        = nullptr;
    IEWindow*   m_ownerWindow = nullptr;
    IERenderer* m_renderer    = nullptr;
    bool        m_prevLMB     = false;

    // ── 색상 상수 ────────────────────────────────────────────────────
    static constexpr SDL_Color kColHeader  = {  60,  60,  70, 255 };
    static constexpr SDL_Color kColHeaderH = {  75,  75,  88, 255 };
    static constexpr SDL_Color kColRowBg   = {  32,  32,  38, 255 };
    static constexpr SDL_Color kColRowAlt  = {  36,  36,  42, 255 };
    static constexpr SDL_Color kColTbBg    = {  28,  28,  34, 255 };
    static constexpr SDL_Color kColTbBor   = {  65,  65,  75, 255 };
    static constexpr SDL_Color kColTbFoc   = {  70, 130, 180, 255 };
    static constexpr SDL_Color kColText    = { 210, 210, 210, 255 };
    static constexpr SDL_Color kColLabel   = { 140, 140, 150, 255 };
};
