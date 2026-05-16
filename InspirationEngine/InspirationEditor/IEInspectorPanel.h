#pragma once
#include "IEEditorPanel.h"

class IEGameObject;

/// <summary>
/// 선택된 오브젝트의 Transform(X/Y/Z)을 편집하는 Inspector 패널.
/// </summary>
class IEInspectorPanel : public IEEditorPanel
{
public:
    IEInspectorPanel();

    virtual void        Draw(IERenderer* r)                                          override;
    virtual void        Update(float dt)                                             override;
    virtual const char* GetTitle()                                         const override { return IELocalize::Get("panel.inspector"); }
    virtual void        SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h) override;
    virtual void        SetFont(IEFont* f)                                           override;
    virtual void        SetOwnerWindow(IEWindow* w)                                  override;
    virtual void        SetRenderer(IERenderer* r)                                   override;

    void SetTarget(IEGameObject* obj);

private:
    static constexpr SDL_Color kColBg     = {  35,  35,  40, 255 };
    static constexpr SDL_Color kColNoSel  = { 110, 110, 110, 255 };

    IEGameObject* m_target = nullptr;

    IEPanelLayout m_layout;
    IESlider      m_sliderX;
    IESlider      m_sliderY;
    IESlider      m_sliderZ;
};
