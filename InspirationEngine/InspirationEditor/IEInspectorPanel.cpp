#include "../InspirationEngine/InspirationEngine.h"
#include "IEInspectorPanel.h"

IEInspectorPanel::IEInspectorPanel()
{
    m_sliderX.SetRange(-2000.0f, 2000.0f);
    m_sliderX.SetValue(0.0f);
    m_sliderX.SetShowValue(true);
    m_sliderX.SetCallback([this](float v) {
        if (m_target == nullptr)
            return;
        auto* t = m_target->GetComponent<IETransformComponent>();
        if (t != nullptr)
            t->SetX(v);
    });

    m_sliderY.SetRange(-2000.0f, 2000.0f);
    m_sliderY.SetValue(0.0f);
    m_sliderY.SetShowValue(true);
    m_sliderY.SetCallback([this](float v) {
        if (m_target == nullptr)
            return;
        auto* t = m_target->GetComponent<IETransformComponent>();
        if (t != nullptr)
            t->SetY(v);
    });

    m_sliderZ.SetRange(-2000.0f, 2000.0f);
    m_sliderZ.SetValue(0.0f);
    m_sliderZ.SetShowValue(true);
    m_sliderZ.SetCallback([this](float v) {
        if (m_target == nullptr)
            return;
        auto* t = m_target->GetComponent<IETransformComponent>();
        if (t != nullptr)
            t->SetZ(v);
    });

    m_layout.LoadJson("Data/UI/inspector_panel.json");
    m_layout.Bind("slider_x", &m_sliderX);
    m_layout.Bind("slider_y", &m_sliderY);
    m_layout.Bind("slider_z", &m_sliderZ);
}

void IEInspectorPanel::SetTarget(IEGameObject* obj)
{
    if (m_target == obj)
        return;

    m_target = obj;

    if (m_target != nullptr)
    {
        auto* t = m_target->GetComponent<IETransformComponent>();
        if (t != nullptr)
        {
            m_sliderX.SetValue(t->GetX());
            m_sliderY.SetValue(t->GetY());
            m_sliderZ.SetValue(t->GetZ());
        }
    }
}

void IEInspectorPanel::SetFont(IEFont* f)        { IEPanel::SetFont(f);        m_layout.SetFont(f); }
void IEInspectorPanel::SetOwnerWindow(IEWindow* w){ IEPanel::SetOwnerWindow(w); m_layout.SetOwnerWindow(w); }
void IEInspectorPanel::SetRenderer(IERenderer* r) { IEPanel::SetRenderer(r);   m_layout.SetRenderer(r); }

void IEInspectorPanel::SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    m_layout.SetContentRect(x, y, w, h);
}

void IEInspectorPanel::Update(float dt)
{
    if (m_target != nullptr)
    {
        auto* t = m_target->GetComponent<IETransformComponent>();
        if (t != nullptr)
        {
            m_sliderX.SetValue(t->GetX());
            m_sliderY.SetValue(t->GetY());
            m_sliderZ.SetValue(t->GetZ());
        }
    }
    m_layout.Update();
}

void IEInspectorPanel::Draw(IERenderer* r)
{
    if (r == nullptr)
        return;

    r->DrawRect(kColBg, m_layout.GetOriginX(), m_layout.GetOriginY(),
                m_layout.GetPanelW(), m_layout.GetPanelH(), SDL_BLENDMODE_NONE);

    if (m_target == nullptr)
    {
        IEFont* font = GetFont();
        if (font != nullptr)
            r->DrawText(font, "No Selection", kColNoSel,
                        m_layout.GetOriginX() + 8, m_layout.GetOriginY() + 8);
        return;
    }

    m_layout.Draw();
}
