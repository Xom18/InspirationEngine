#pragma once
#include "../InspirationEngine/InspirationEngine.h"
#include "IEEditorPanel.h"
#include <memory>
#include <string>

/// <summary>
/// 패널이 메인 창 밖으로 드래그됐을 때 생성되는 독립 SDL 창.
/// CallXButton() 호출 시 IECore::RequestRemoveWindow 로 안전하게 소멸.
/// </summary>
class IEFloatingPanelWindow : public IEWindow
{
public:
    void Init(std::unique_ptr<IEEditorPanel> panel, IEFont* font, const std::string& windowId);

    virtual void Draw()                          override;
    virtual void Update(float dt)                override;
    virtual void CallXButton()                   override;
    virtual void OnResize(int32_t w, int32_t h)  override;

    const std::string& GetWindowId() const { return m_windowId; }

private:
    std::unique_ptr<IEEditorPanel> m_panel;
    IEFont*     m_font     = nullptr;
    std::string m_windowId;
};
