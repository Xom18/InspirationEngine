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
    virtual void OnResize(int32_t w, int32_t h) override;

    void InitWindow(IEFont* font, IEAtlasEditorWindow* atlasEditor);

    static constexpr int32_t kMinW = 800;
    static constexpr int32_t kMinH = 500;

private:
    void InitScene();
    void DrawViewport();
    void DrawViewportGrid(IERenderer* r);
    void UpdateViewport();
    void SelectAtViewportPos(int32_t vx, int32_t vy);
    void OnFileBrowserSelect(const std::string& path);

    // Fixed layout dimensions
    static constexpr int32_t kMenuH    = 40;
    static constexpr int32_t kBrowserW = 220;

    // Dynamic layout — derived from current window size
    int32_t ViewportX() { return kBrowserW; }
    int32_t ViewportY() { return kMenuH; }
    int32_t ViewportW() { return GetWidth() - kBrowserW; }
    int32_t ViewportH() { return GetHeight() - kMenuH; }

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
