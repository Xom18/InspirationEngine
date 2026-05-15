#include "IEFileBrowserPanel.h"
#include "IEAtlasEditorWindow.h"
#include <filesystem>
#include <algorithm>
#include <cctype>

namespace fs = std::filesystem;

IEFileBrowserPanel::IEFileBrowserPanel()
{
    m_fileBrowser.SetCallback([this](const std::string& path) { OnSelect(path); });
}

void IEFileBrowserPanel::SetFont(IEFont* f)
{
    m_fileBrowser.SetFont(f);
}

void IEFileBrowserPanel::SetOwnerWindow(IEWindow* w)
{
    m_fileBrowser.SetOwnerWindow(w);
}

void IEFileBrowserPanel::SetRenderer(IERenderer* r)
{
    m_fileBrowser.SetRenderer(r);
}

void IEFileBrowserPanel::SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    m_fileBrowser.SetRect(x, y, w, h);
}

void IEFileBrowserPanel::Update(float dt)
{
    m_fileBrowser.Update();
}

void IEFileBrowserPanel::Draw(IERenderer* r)
{
    m_fileBrowser.Draw();
}

void IEFileBrowserPanel::OnSelect(const std::string& path)
{
    if (m_atlasEditor == nullptr)
        return;
    std::string ext = fs::path(path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (ext == ".png" || ext == ".json")
        m_atlasEditor->OpenWithFile(path);
}
