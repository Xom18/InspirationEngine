#pragma once
#include "../InspirationEngine/InspirationEngine.h"
#include "IEDockedPanel.h"
#include "IEViewportPanel.h"
#include "IEFileBrowserPanel.h"
#include "IEHierarchy.h"
#include "IECameraPanel.h"
#include "IEInspectorPanel.h"
#include <vector>
#include <string>
#include <atomic>

class IEAtlasEditorWindow;

class IEMainEditorWindow : public IEWindow
{
public:
    virtual void Draw()                          override;
    virtual void Update(float deltaTime)         override;
    virtual void CallXButton()                   override;
    virtual void OnResize(int32_t w, int32_t h)  override;

    void InitWindow(IEFont* font, IEAtlasEditorWindow* atlasEditor);

    static constexpr int32_t kMinW = 800;
    static constexpr int32_t kMinH = 500;

private:
    void InitPanels(IEFont* font, IEAtlasEditorWindow* atlasEditor);
    void LayoutPanels();
    void ProcessUndock();

    static constexpr int32_t  kMenuH    = 40;
    static constexpr SDL_Color kColBg   = {  30,  30,  30, 255 };
    static constexpr SDL_Color kColMenu = {  45,  45,  50, 255 };
    static constexpr SDL_Color kColSep  = {  65,  65,  70, 255 };
    static constexpr SDL_Color kColText = { 200, 200, 200, 255 };

    IEAtlasEditorWindow* m_atlasEditor = nullptr;
    IEFont*              m_font        = nullptr;

    IEButton m_btnAtlas;

    // 패널 리스트 (back = 최상위 z-order)
    std::vector<IEDockedPanel> m_panels;

    // 빠른 접근용 raw 포인터 (소유권은 m_panels 안에 있음)
    IEViewportPanel*   m_vpPanel     = nullptr;
    IECameraPanel*     m_camPanel    = nullptr;
    IEHierarchy* m_entityPanel = nullptr;
    IEInspectorPanel*  m_inspPanel   = nullptr;

    // 부동 창 ID 카운터 (스레드 안전)
    std::atomic<int32_t> m_floatIdCounter{0};
};
