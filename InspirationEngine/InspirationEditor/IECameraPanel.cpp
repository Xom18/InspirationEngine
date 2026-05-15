#include "IECameraPanel.h"
#include <cstdio>

IECameraPanel::IECameraPanel()
{
    m_sliderZoom.SetRange(0.05f, 20.0f);
    m_sliderZoom.SetValue(1.0f);
    m_sliderZoom.SetShowValue(true);
    m_sliderZoom.SetCallback([this](float v) {
        if (m_camera != nullptr)
            m_camera->SetZoom(v);
    });

    m_sliderX.SetRange(-2000.0f, 2000.0f);
    m_sliderX.SetValue(0.0f);
    m_sliderX.SetShowValue(true);
    m_sliderX.SetCallback([this](float v) {
        if (m_camera != nullptr)
            m_camera->SetPosition(v, m_camera->GetY());
    });

    m_sliderY.SetRange(-2000.0f, 2000.0f);
    m_sliderY.SetValue(0.0f);
    m_sliderY.SetShowValue(true);
    m_sliderY.SetCallback([this](float v) {
        if (m_camera != nullptr)
            m_camera->SetPosition(m_camera->GetX(), v);
    });
}

void IECameraPanel::SetCamera(IECamera* camera)
{
    m_camera = camera;
    if (camera != nullptr)
    {
        m_sliderZoom.SetValue(camera->GetZoom());
        m_sliderX.SetValue(camera->GetX());
        m_sliderY.SetValue(camera->GetY());
    }
}

void IECameraPanel::SetFont(IEFont* f)
{
    m_font = f;
    m_sliderZoom.SetFont(f);
    m_sliderX.SetFont(f);
    m_sliderY.SetFont(f);
}

void IECameraPanel::SetOwnerWindow(IEWindow* w)
{
    m_ownerWindow = w;
    m_sliderZoom.SetOwnerWindow(w);
    m_sliderX.SetOwnerWindow(w);
    m_sliderY.SetOwnerWindow(w);
}

void IECameraPanel::SetRenderer(IERenderer* r)
{
    m_renderer = r;
    m_sliderZoom.SetRenderer(r);
    m_sliderX.SetRenderer(r);
    m_sliderY.SetRenderer(r);
}

void IECameraPanel::SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;
    LayoutSliders();
}

void IECameraPanel::LayoutSliders()
{
    int32_t sliderW = m_w - kPadX * 2 - kLabelW;
    if (sliderW < 20)
        sliderW = 20;

    int32_t sx = m_x + kPadX + kLabelW;
    m_sliderZoom.SetRect(sx, m_y + kPadY,            sliderW, 20);
    m_sliderX.SetRect   (sx, m_y + kPadY + kRowH,     sliderW, 20);
    m_sliderY.SetRect   (sx, m_y + kPadY + kRowH * 2, sliderW, 20);
}

void IECameraPanel::Update(float dt)
{
    if (m_camera != nullptr)
    {
        // 뷰포트 입력(RMB 드래그, 스크롤)이 카메라를 직접 바꾸므로 슬라이더 동기화
        m_sliderZoom.SetValue(m_camera->GetZoom());
        m_sliderX.SetValue(m_camera->GetX());
        m_sliderY.SetValue(m_camera->GetY());
    }
    m_sliderZoom.Update();
    m_sliderX.Update();
    m_sliderY.Update();
}

void IECameraPanel::Draw(IERenderer* r)
{
    if (r == nullptr)
        return;

    r->DrawRect(kColBg, m_x, m_y, m_w, m_h, SDL_BLENDMODE_NONE);

    if (m_font != nullptr)
    {
        r->DrawText(m_font, "Zoom", kColText, m_x + kPadX, m_y + kPadY + 3);
        r->DrawText(m_font, "X",    kColText, m_x + kPadX, m_y + kPadY + kRowH + 3);
        r->DrawText(m_font, "Y",    kColText, m_x + kPadX, m_y + kPadY + kRowH * 2 + 3);

        if (m_camera != nullptr)
        {
            char buf[32];
            std::snprintf(buf, sizeof(buf), "type: TopView");
            r->DrawText(m_font, buf, kColText, m_x + kPadX, m_y + kPadY + kRowH * 3 + 8);
        }
    }

    m_sliderZoom.Draw();
    m_sliderX.Draw();
    m_sliderY.Draw();
}
