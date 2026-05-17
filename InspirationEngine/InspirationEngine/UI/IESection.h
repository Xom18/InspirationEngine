#pragma once
#include <string>
#include <vector>
#include <memory>
#include "IEUIBase.h"

class IETextBox;
class IELabel;
class IESlider;
class IEDropdown;

/// <summary>
/// 컴포짓 패턴 기반 재귀 가능 UI 섹션 컨테이너.
/// IEUIBase를 상속하므로 Panel의 vector<IEUIBase> 자식으로 직접 추가 가능.
///
/// AddSection / AddTextBox / AddSlider / AddLabel / AddDropdown 으로 자식 추가.
/// 내부적으로 IERow를 생성해 레이블 + 위젯을 감싼 뒤 m_children에 저장.
/// Layout(x, y, w) 후 Draw / Update 사용.
/// </summary>
class IESection : public IEUIBase
{
public:
    static constexpr int32_t kHeaderH  = 22;
    static constexpr int32_t kRowH     = 24;
    static constexpr int32_t kPadY     = 2;
    static constexpr int32_t kIndentX  = 8;
    static constexpr int32_t kMarginR  = 4;

    void SetTitle(const std::string& title) { m_title = title; }
    const std::string& GetTitle() const     { return m_title; }

    void SetCollapsible(bool c) { m_collapsible = c; }
    void SetCollapsed(bool c)   { m_collapsed   = c; }
    bool IsCollapsed() const    { return m_collapsed; }

    void SetHeaderLabel(const std::string& text) { m_headerLabel = text; }
    void SetHeaderLabelColor(SDL_Color col)       { m_headerLabelColor = col; }

    // ── 자식 추가 ─────────────────────────────────────────────────────

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

    // ── 레이아웃 / 렌더 / 입력 ───────────────────────────────────────

    /// <summary>
    /// 모든 자식의 절대 위치를 계산. 반환값 = 이 섹션이 차지한 총 픽셀 높이.
    /// </summary>
    int32_t Layout(int32_t x, int32_t y, int32_t w) override;

    int32_t GetTotalHeight() const { return m_totalH; }

    void Draw()        override;
    void Update()      override;

    /// <summary>열린 Dropdown 팝업을 최상위에 재렌더 (z-order 보정). 재귀 전파.</summary>
    void DrawOverlay() override;

private:
    bool CheckHeaderClick();

    // Layout 상태
    int32_t  m_x = 0, m_y = 0, m_w = 0;
    int32_t  m_totalH = kHeaderH;
    SDL_Rect m_headerRect{};

    std::string m_title;
    std::string m_headerLabel;
    SDL_Color   m_headerLabelColor  = { 140, 140, 150, 255 };
    static constexpr float kHeaderLabelRatio = 0.45f;

    bool m_collapsible = true;
    bool m_collapsed   = false;
    bool m_prevLMB     = false;

    // (m_children, m_font, m_renderer, m_ownerWindow 는 IEUIBase에서 상속)

    static constexpr SDL_Color kColHeader  = {  60,  60,  70, 255 };
    static constexpr SDL_Color kColHeaderH = {  75,  75,  88, 255 };
    static constexpr SDL_Color kColText    = { 210, 210, 210, 255 };
    static constexpr SDL_Color kColLabel   = { 140, 140, 150, 255 };
};
