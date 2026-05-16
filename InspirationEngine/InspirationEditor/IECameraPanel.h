#pragma once
#include "IEEditorPanel.h"

/// <summary>
/// 카메라 파라미터 조정 패널. zoom / position 슬라이더.
/// </summary>
class IECameraPanel : public IEEditorPanel
{
public:
    IECameraPanel();

    virtual void        Draw(IERenderer* r)                                         override;
    virtual void        Update(float dt)                                            override;
    virtual const char* GetTitle()                                       const override { return IELocalize::Get("panel.camera"); }
    virtual void        SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h) override;
    virtual void        SetFont(IEFont* f)                                          override;
    virtual void        SetOwnerWindow(IEWindow* w)                                 override;
    virtual void        SetRenderer(IERenderer* r)                                  override;

    void SetCamera(IECamera* camera);

private:
    static constexpr SDL_Color kColBg   = {  35,  35,  40, 255 };
    static constexpr SDL_Color kColText = { 180, 180, 180, 255 };

    IECamera*     m_camera = nullptr;
    IEPanelLayout m_layout;

    IESlider m_sliderZoom;
    IESlider m_sliderX;
    IESlider m_sliderY;
};
