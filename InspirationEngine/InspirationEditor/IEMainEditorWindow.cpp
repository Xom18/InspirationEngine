#include "IEMainEditorWindow.h"
#include "IEAtlasEditorWindow.h"
#include <filesystem>
#include <algorithm>
#include <cctype>

namespace fs = std::filesystem;

static SDL_Color kColBg       = {  30,  30,  30, 255 };
static SDL_Color kColMenu     = {  45,  45,  50, 255 };
static SDL_Color kColBrowser  = {  40,  40,  45, 255 };
static SDL_Color kColViewport = {  25,  25,  25, 255 };
static SDL_Color kColSep      = {  65,  65,  70, 255 };
static SDL_Color kColText     = { 200, 200, 200, 255 };
static SDL_Color kColTbBor    = {  80,  80,  85, 255 };

void IEMainEditorWindow::InitWindow(IEFont* font, IEAtlasEditorWindow* atlasEditor)
{
    m_font        = font;
    m_atlasEditor = atlasEditor;

    IERenderer* r = GetRenderer(0);

    m_btnAtlas.SetFont(font);
    m_btnAtlas.SetRenderer(r);
    m_btnAtlas.SetRect(8, 6, 100, 28);
    m_btnAtlas.SetLabel("Atlas Editor");
    m_btnAtlas.SetOwnerWindow(this);
    m_btnAtlas.SetCallback([this]() {
        if (m_atlasEditor != nullptr)
            m_atlasEditor->ShowWindow();
    });

    m_fileBrowser.SetRenderer(r);
    m_fileBrowser.SetFont(font);
    m_fileBrowser.SetOwnerWindow(this);
    constexpr int32_t kLabelH = 24;
    m_fileBrowser.SetRect(0, kMenuH + kLabelH, kBrowserW, kWinH - kMenuH - kLabelH);
    m_fileBrowser.SetCallback([this](const std::string& path) { OnFileBrowserSelect(path); });
}

void IEMainEditorWindow::CallXButton()
{
    IECore::StopEngine();
}

void IEMainEditorWindow::OnFileBrowserSelect(const std::string& path)
{
    if (m_atlasEditor == nullptr)
        return;

    std::string ext = fs::path(path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (ext == ".png" || ext == ".json")
        m_atlasEditor->OpenWithFile(path);
}

void IEMainEditorWindow::Update(float /*deltaTime*/)
{
    m_btnAtlas.Update();
    m_fileBrowser.Update();
}

void IEMainEditorWindow::Draw()
{
    IERenderer* r = GetRenderer(0);
    if (r == nullptr)
        return;

    // Backgrounds
    r->DrawRect(kColBg,       0,           0,       kWinW,    kWinH,    SDL_BLENDMODE_NONE);
    r->DrawRect(kColMenu,     0,           0,       kWinW,    kMenuH,   SDL_BLENDMODE_NONE);
    r->DrawRect(kColBrowser,  0,           kMenuH,  kBrowserW, kViewportH, SDL_BLENDMODE_NONE);
    r->DrawRect(kColViewport, kViewportX,  kMenuH,  kViewportW, kViewportH, SDL_BLENDMODE_NONE);

    // Separator
    r->DrawLine(kColSep, kBrowserW, 0, kBrowserW, kWinH);
    r->DrawLine(kColSep, 0, kMenuH, kWinW, kMenuH);

    // Menu bar
    m_btnAtlas.Draw();

    // Resource browser label
    r->DrawText(m_font, "Resources", kColText, 6, kMenuH + 6);
    r->DrawLine(kColSep, 0, kMenuH + 22, kBrowserW, kMenuH + 22);

    // File browser (starts below label)
    m_fileBrowser.Draw();

    // Viewport placeholder
    int32_t cx = kViewportX + kViewportW / 2 - 60;
    int32_t cy = kViewportY + kViewportH / 2 - 8;
    r->DrawText(m_font, "Viewport (E-3)", { 80, 80, 80, 255 }, cx, cy);
}
