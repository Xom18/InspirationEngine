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
    void OnFileBrowserSelect(const std::string& path);

    static constexpr int32_t kWinW    = 1280;
    static constexpr int32_t kWinH    = 720;
    static constexpr int32_t kMenuH   = 40;
    static constexpr int32_t kBrowserW = 220;
    static constexpr int32_t kViewportX = kBrowserW;
    static constexpr int32_t kViewportY = kMenuH;
    static constexpr int32_t kViewportW = kWinW - kBrowserW;
    static constexpr int32_t kViewportH = kWinH - kMenuH;

    IEAtlasEditorWindow* m_atlasEditor = nullptr;
    IEFont*              m_font        = nullptr;

    IEFileBrowser m_fileBrowser;
    IEButton      m_btnAtlas;
};
