#pragma once
#include "IEEditorPanel.h"
#include <vector>
#include <memory>

class IEGameObject;

/// <summary>
/// 선택된 오브젝트의 컴포넌트를 IESection 기반 섹션으로 편집하는 Inspector 패널.
/// SetTarget() 시 컴포넌트 구성에 따라 섹션을 동적으로 재구성.
/// </summary>
class IEInspectorPanel : public IEEditorPanel
{
public:
    /// <summary>패널 콘텐츠 렌더링</summary>
    virtual void        Draw(IERenderer* r)                                          override;

    /// <summary>대상 오브젝트 동기화 및 입력 처리</summary>
    virtual void        Update(float dt)                                             override;

    /// <summary>패널 타이틀 문자열 반환</summary>
    virtual const char* GetTitle()                                         const override { return IELocalize::Get("panel.inspector"); }

    /// <summary>패널 콘텐츠 영역 설정</summary>
    virtual void        SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h) override;

    /// <summary>폰트 설정 — 내부 섹션에 전파</summary>
    virtual void        SetFont(IEFont* f)                                           override;

    /// <summary>소유 창 설정 — 내부 섹션에 전파</summary>
    virtual void        SetOwnerWindow(IEWindow* w)                                  override;

    /// <summary>렌더러 설정 — 내부 섹션에 전파</summary>
    virtual void        SetRenderer(IERenderer* r)                                   override;

    /// <summary>편집 대상 오브젝트 설정 — 컴포넌트에 따라 섹션 재구성</summary>
    void SetTarget(IEGameObject* obj);

private:
    void RebuildSections();
    void RelayoutSections();
    void SyncFromTarget();
    void ApplyFocusedInput();

    void PropagateContextToSections();

    IEScrollView                            m_scroll;
    std::vector<std::unique_ptr<IESection>> m_sections;
    IEGameObject*                           m_target = nullptr;
    int32_t m_x = 0, m_y = 0, m_w = 1, m_h = 1;

    // 위젯 raw 포인터 캐시 (소유권은 m_sections 내부)
    IETextBox*  m_tbName    = nullptr;
    IESlider*   m_slX       = nullptr;
    IESlider*   m_slY       = nullptr;
    IESlider*   m_slZ       = nullptr;
    IESlider*   m_slRot     = nullptr;
    IESlider*   m_slSx      = nullptr;
    IESlider*   m_slSy      = nullptr;
    IETextBox*  m_tbAtlas   = nullptr;
    IETextBox*  m_tbTile    = nullptr;
    IEDropdown* m_ddCamType = nullptr;
    IESlider*   m_slCamZoom = nullptr;

    static constexpr SDL_Color kColBg    = {  35,  35,  40, 255 };
    static constexpr SDL_Color kColNoSel = { 110, 110, 110, 255 };
};
