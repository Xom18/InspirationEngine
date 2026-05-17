#pragma once
#include <string>
#include "IEUIBase.h"

/// <summary>
/// 레이블 + 단일 위젯 래퍼 행. IESection의 자식으로 사용.
/// 왼쪽 kLabelW 픽셀에 레이블 텍스트, 나머지 우측에 위젯(m_children[0]) 렌더.
/// </summary>
class IEUIRow : public IEUIBase
{
public:
    void SetLabel(const std::string& label) { m_label = label; }
    const std::string& GetLabel() const     { return m_label; }

    /// <summary>레이블 없는 전체 너비 위젯 행 여부. Label 타입 등에서 사용.</summary>
    void SetFullWidth(bool fw) { m_fullWidth = fw; }

    // ── IEUIBase 오버라이드 ─────────────────────────────────────────

    bool IsRow()               const override { return true; }
    void SetRowParity(bool even)     override { m_even = even; }

    /// <summary>
    /// 자식 위젯에 절대 좌표 할당. 반환값 = 행 높이(kRowH).
    /// </summary>
    int32_t Layout(int32_t x, int32_t y, int32_t w) override;

    /// <summary>행 배경 + 레이블 + 자식 위젯 Draw.</summary>
    void Draw()        override;

    /// <summary>자식 위젯의 DrawOverlay 전파 (Dropdown 팝업 z-order용).</summary>
    void DrawOverlay() override;

    void Update() override;

    // ── 자식 추가 헬퍼 (IESection::AddXxx에서만 사용) ───────────────

    template<typename T>
    T* SetWidget(std::unique_ptr<T> widget)
    {
        return AddChild(std::move(widget));
    }

    static constexpr int32_t kLabelW = 72;
    static constexpr int32_t kRowH   = 24;

private:
    std::string m_label;
    bool        m_even      = true;
    bool        m_fullWidth = false;  // true 시 레이블 없이 전체 너비 위젯

    static constexpr SDL_Color kColRowBg  = {  32,  32,  38, 255 };
    static constexpr SDL_Color kColRowAlt = {  36,  36,  42, 255 };
    static constexpr SDL_Color kColLabel  = { 140, 140, 150, 255 };
};
