#include "IECameraPanel.h"
#include <cstdio>

IECameraPanel::IECameraPanel()
{
    m_sliderZoom.SetRange(0.05f, 20.0f);
    m_sliderZoom.SetValue(1.0f);
    m_sliderZoom.SetShowValue(true);
    m_sliderZoom.SetCallback([this](float v) {
        if (m_cameraObj == nullptr)
            return;
        auto* cc = m_cameraObj->GetComponent<IECameraComponent>();
        if (cc != nullptr)
            cc->SetZoom(v);
    });

    m_sliderX.SetRange(-2000.0f, 2000.0f);
    m_sliderX.SetValue(0.0f);
    m_sliderX.SetShowValue(true);
    m_sliderX.SetCallback([this](float v) {
        if (m_cameraObj == nullptr)
            return;
        auto* t = m_cameraObj->GetComponent<IETransformComponent>();
        if (t != nullptr)
            t->SetX(v);
    });

    m_sliderY.SetRange(-2000.0f, 2000.0f);
    m_sliderY.SetValue(0.0f);
    m_sliderY.SetShowValue(true);
    m_sliderY.SetCallback([this](float v) {
        if (m_cameraObj == nullptr)
            return;
        auto* t = m_cameraObj->GetComponent<IETransformComponent>();
        if (t != nullptr)
            t->SetY(v);
    });

    m_layout.LoadJson("Data/UI/camera_panel.json");
    m_layout.Bind("slider_zoom", &m_sliderZoom);
    m_layout.Bind("slider_x",    &m_sliderX);
    m_layout.Bind("slider_y",    &m_sliderY);
}

void IECameraPanel::SetCameraObject(IEGameObject* obj)
{
    m_cameraObj = obj;
    if (obj == nullptr)
        return;

    auto* cc = obj->GetComponent<IECameraComponent>();
    auto* t  = obj->GetComponent<IETransformComponent>();
    if (cc != nullptr)
        m_sliderZoom.SetValue(cc->GetZoom());
    if (t != nullptr)
    {
        m_sliderX.SetValue(t->GetX());
        m_sliderY.SetValue(t->GetY());
    }
}

void IECameraPanel::SetFont(IEFont* f)          { m_layout.SetFont(f); }
void IECameraPanel::SetOwnerWindow(IEWindow* w) { m_layout.SetOwnerWindow(w); }
void IECameraPanel::SetRenderer(IERenderer* r)  { m_layout.SetRenderer(r); }

void IECameraPanel::SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    m_layout.SetContentRect(x, y, w, h);
}

void IECameraPanel::Update(float /*dt*/)
{
    if (m_cameraObj != nullptr)
    {
        auto* cc = m_cameraObj->GetComponent<IECameraComponent>();
        auto* t  = m_cameraObj->GetComponent<IETransformComponent>();
        if (cc != nullptr)
            m_sliderZoom.SetValue(cc->GetZoom());
        if (t != nullptr)
        {
            m_sliderX.SetValue(t->GetX());
            m_sliderY.SetValue(t->GetY());
        }
    }
    m_layout.Update();
}

void IECameraPanel::Draw(IERenderer* r)
{
    if (r == nullptr)
        return;

    r->DrawRect(kColBg, m_layout.GetOriginX(), m_layout.GetOriginY(),
                m_layout.GetPanelW(), m_layout.GetPanelH(), SDL_BLENDMODE_NONE);

    m_layout.Draw();

    IEFont* font = m_layout.GetFont();
    if (font != nullptr && m_cameraObj != nullptr)
    {
        auto* cc = m_cameraObj->GetComponent<IECameraComponent>();
        if (cc != nullptr)
        {
            int32_t px = m_layout.GetOriginX() + 8;
            int32_t py = m_layout.GetOriginY() + 92;
            r->DrawText(font, IELocalize::Get("label.camera_type"), kColText, px, py);

            char typeBuf[32];
            std::snprintf(typeBuf, sizeof(typeBuf), " %s", IECameraComponent::TypeName(cc->GetType()));
            r->DrawText(font, typeBuf, kColText, px + 80, py);
        }
    }
}
