#pragma once
#include "IEEditorPanel.h"

class IEGameObject;

/// <summary>
/// 선택된 오브젝트의 이름·Transform·Tile 컴포넌트를 편집하는 Inspector 패널
/// </summary>
class IEInspectorPanel : public IEEditorPanel
{
public:
    IEInspectorPanel();

    /// <summary>패널 콘텐츠 렌더링</summary>
    virtual void        Draw(IERenderer* r)                                          override;

    /// <summary>대상 오브젝트 동기화 및 입력 처리</summary>
    virtual void        Update(float dt)                                             override;

    /// <summary>패널 타이틀 문자열 반환</summary>
    virtual const char* GetTitle()                                         const override { return IELocalize::Get("panel.inspector"); }

    /// <summary>패널 콘텐츠 영역 설정</summary>
    virtual void        SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h) override;

    /// <summary>폰트 설정 — 내부 위젯에 전파</summary>
    virtual void        SetFont(IEFont* f)                                           override;

    /// <summary>소유 창 설정 — 내부 위젯에 전파</summary>
    virtual void        SetOwnerWindow(IEWindow* w)                                  override;

    /// <summary>렌더러 설정 — 내부 위젯에 전파</summary>
    virtual void        SetRenderer(IERenderer* r)                                   override;

    /// <summary>편집 대상 오브젝트 설정</summary>
    void SetTarget(IEGameObject* obj);

private:
    void SetFocus(IETextBox* tb);
    bool HitTest(const SDL_Rect& r, int32_t mx, int32_t my) const;
    void SyncFloat(IETextBox& tb, float value);
    void SyncStr(IETextBox& tb, const std::string& value);

    static constexpr SDL_Color kColBg    = {  35,  35,  40, 255 };
    static constexpr SDL_Color kColNoSel = { 110, 110, 110, 255 };
    static constexpr SDL_Color kColTbBg  = {  30,  30,  35, 255 };
    static constexpr SDL_Color kColTbBor = {  70,  70,  80, 255 };
    static constexpr SDL_Color kColTbFoc = {  70, 130, 180, 255 };

    IEGameObject* m_target     = nullptr;
    IETextBox*    m_focusedBox = nullptr;
    bool          m_prevLMB    = false;

    IEPanelLayout m_layout;

    // Name
    IETextBox m_tbName;

    // Transform
    IETextBox m_tbX;
    IETextBox m_tbY;
    IETextBox m_tbZ;
    IETextBox m_tbRot;
    IETextBox m_tbScaleX;
    IETextBox m_tbScaleY;

    // Tile component
    IETextBox m_tbAtlas;
    IETextBox m_tbTile;
};
