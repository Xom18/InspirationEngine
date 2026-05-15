#include "IEFloatingPanel.h"
#include "../InspirationEngine.h"

void IEFloatingPanel::Init(std::unique_ptr<IEPanel> panel, IEFont* font, const std::string& windowId)
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

void IEFloatingPanel::OnResize(int32_t w, int32_t h)
{
    if (m_panel != nullptr)
        m_panel->SetContentRect(0, 0, w, h);
}

void IEFloatingPanel::Update(float dt)
{
    if (m_panel != nullptr)
        m_panel->Update(dt);
}

void IEFloatingPanel::Draw()
{
    IERenderer* r = GetRenderer(0);
    if (r == nullptr || m_panel == nullptr)
        return;
    m_panel->Draw(r);
}

void IEFloatingPanel::CallXButton()
{
    IECore::RequestRemoveWindow(m_windowId.c_str());
}
