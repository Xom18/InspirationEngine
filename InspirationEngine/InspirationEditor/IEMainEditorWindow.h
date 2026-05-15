#pragma once
#include "../InspirationEngine/InspirationEngine.h"
#include <string>

class IEAtlasEditorWindow;

class IEMainEditorWindow : public IEWindow
{
public:
    virtual void Draw() override;
    virtual void Update(float deltaTime) override;
    virtual void CallXButton() override;

    void InitWindow(IEFont* font, IEAtlasEditorWindow* atlasEditor);

private:
    void InitScene();
    void DrawViewport();
    void DrawViewportGrid(IERenderer* r);
    void UpdateViewport();
    void SelectAtViewportPos(int32_t vx, int32_t vy);
    void OnFileBrowserSelect(const std::string& path);

    static constexpr int32_t kWinW     = 1280;
    static constexpr int32_t kWinH     = 720;
    static constexpr int32_t kMenuH    = 40;
    static constexpr int32_t kBrowserW = 220;
    static constexpr int32_t kViewportX = kBrowserW;
    static constexpr int32_t kViewportY = kMenuH;
    static constexpr int32_t kViewportW = kWinW - kBrowserW;
    static constexpr int32_t kViewportH = kWinH - kMenuH;

    IEAtlasEditorWindow* m_atlasEditor = nullptr;
    IEFont*              m_font        = nullptr;

    // UI
    IEFileBrowser m_fileBrowser;
    IEButton      m_btnAtlas;

    // Scene / camera
    IEScene          m_scene;
    IECameraTopView* m_camera      = nullptr;  // observer, owned by m_scene
    IEGameObject*    m_selectedObj = nullptr;

    // Viewport interaction (RMB drag = pan, LMB click = select, scroll = zoom)
    bool  m_vpDragging   = false;
    float m_vpDragStartX = 0.0f;
    float m_vpDragStartY = 0.0f;
    float m_camStartX    = 0.0f;
    float m_camStartY    = 0.0f;
    bool  m_vpPrevLMB    = false;
    bool  m_vpPrevRMB    = false;
};
