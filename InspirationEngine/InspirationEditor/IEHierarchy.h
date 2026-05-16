#pragma once
#include "IEEditorPanel.h"

/// <summary>
/// 씬 오브젝트 목록 패널. 클릭 시 선택 연동 예정.
/// </summary>
class IEHierarchy : public IEEditorPanel
{
public:
    virtual void        Draw(IERenderer* r)                                          override;
    virtual void        Update(float dt)                                             override;
    virtual const char* GetTitle()                                         const override { return IELocalize::Get("panel.hierarchy"); }
    virtual void        SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h) override;
    virtual void        SetFont(IEFont* f)                                           override;
    virtual void        SetOwnerWindow(IEWindow* w)                                  override;
    virtual void        SetRenderer(IERenderer* r)                                   override;

    void    SetScene(IEScene* scene) { m_scene = scene; }
    int32_t GetSelectedIndex() const { return m_list.GetSelectedIndex(); }

private:
    IEScene*   m_scene = nullptr;
    IEListView m_list;
};
