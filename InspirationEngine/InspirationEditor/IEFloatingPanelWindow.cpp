#include "IEFloatingPanelWindow.h"

void IEFloatingPanelWindow::Init(std::unique_ptr<IEEditorPanel> panel, IEFont* font, const std::string& windowId)
{
    m_panel    = std::move(panel);
    m_font     = font;
    m_windowId = windowId;

    if (m_panel != nullptr)
    {
        m_panel->SetFont(m_font);
        m_panel->SetOwnerWindow(this);
        m_panel->SetRenderer(GetRenderer(0));
        m_panel->SetContentRect(0, 0, GetWidth(), GetHeight());
    }
}

void IEFloatingPanelWindow::OnResize(int32_t w, int32_t h)
{
    if (m_panel != nullptr)
        m_panel->SetContentRect(0, 0, w, h);
}

void IEFloatingPanelWindow::Update(float dt)
{
    if (m_panel != nullptr)
        m_panel->Update(dt);
}

void IEFloatingPanelWindow::Draw()
{
    IERenderer* r = GetRenderer(0);
    if (r == nullptr || m_panel == nullptr)
        return;
    m_panel->Draw(r);
}

void IEFloatingPanelWindow::CallXButton()
{
    IECore::RequestRemoveWindow(m_windowId.c_str());
}
