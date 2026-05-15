#pragma once
#include "IEEditorPanel.h"
#include <cmath>

/// <summary>
/// 씬 뷰포트 패널. RMB 패닝 / 스크롤 줌 / LMB 선택 지원.
/// </summary>
class IEViewportPanel : public IEEditorPanel
{
public:
    IEViewportPanel();

    virtual void        Draw(IERenderer* r)                                override;
    virtual void        Update(float dt)                                   override;
    virtual const char* GetTitle()                               const override { return "Viewport"; }
    virtual void        SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h) override;
    virtual void        SetFont(IEFont* f)                                 override { m_font = f; }
    virtual void        SetOwnerWindow(IEWindow* w)                        override { m_ownerWindow = w; }
    virtual void        SetRenderer(IERenderer* r)                         override { m_renderer = r; }

    IEScene&         GetScene()          { return m_scene; }
    IECameraTopView* GetCamera()         { return m_camera; }
    IEGameObject*    GetSelectedObject() { return m_selectedObj; }

private:
    void DrawGrid(IERenderer* r);
    void UpdateInput();
    void SelectAt(int32_t vx, int32_t vy);

    static SDL_Color kColVp;
    static SDL_Color kColGrid;
    static SDL_Color kColOrigin;
    static SDL_Color kColSel;
    static SDL_Color kColText;

    IEScene          m_scene;
    IECameraTopView* m_camera      = nullptr;
    IEGameObject*    m_selectedObj = nullptr;

    IEFont*     m_font        = nullptr;
    IEWindow*   m_ownerWindow = nullptr;
    IERenderer* m_renderer    = nullptr;

    int32_t m_x = 0, m_y = 0, m_w = 1, m_h = 1;

    bool  m_vpDragging   = false;
    float m_vpDragStartX = 0.0f;
    float m_vpDragStartY = 0.0f;
    float m_camStartX    = 0.0f;
    float m_camStartY    = 0.0f;
    bool  m_vpPrevLMB    = false;
    bool  m_vpPrevRMB    = false;
};
