#include "../InspirationEngine.h"
#include <filesystem>
#include <algorithm>
#include <cctype>

namespace fs = std::filesystem;

IEFileBrowser::IEFileBrowser()
{
    m_treeView.SetCallback([this](IETreeNode* node) {
        auto it = m_nodePaths.find(node);
        if (it == m_nodePaths.end())
            return;
        m_selectedPath = it->second;
        if (m_callback)
            m_callback(m_selectedPath);
    });
}

void IEFileBrowser::SetRenderer(IERenderer* r)
{
    IEUIBase::SetRenderer(r);
    m_treeView.SetRenderer(r);
}

void IEFileBrowser::SetRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    m_treeView.SetRect(x, y, w, h);
}

void IEFileBrowser::SetFont(IEFont* font)
{
    m_treeView.SetFont(font);
}

void IEFileBrowser::SetOwnerWindow(IEWindow* window)
{
    m_treeView.SetOwnerWindow(window);
}

void IEFileBrowser::SetRootPath(const char* path)
{
    m_rootPath = path ? path : "";
    Refresh();
}

void IEFileBrowser::SetFilter(const char* ext)
{
    m_filter = ext ? ext : "";
}

void IEFileBrowser::SetCallback(std::function<void(const std::string&)> callback)
{
    m_callback = std::move(callback);
}

void IEFileBrowser::Refresh()
{
    m_treeView.Clear();
    m_nodePaths.clear();
    m_selectedPath.clear();

    if (m_rootPath.empty())
        return;

    std::error_code ec;
    if (!fs::exists(m_rootPath, ec) || ec)
        return;

    fs::path root(m_rootPath);
    IETreeNode* rootNode = m_treeView.AddRootNode(root.filename().string().c_str());
    m_nodePaths[rootNode] = m_rootPath;
    rootNode->SetExpanded(true);
    BuildTree(rootNode, m_rootPath);
}

void IEFileBrowser::BuildTree(IETreeNode* parent, const std::string& dirPath)
{
    std::vector<fs::directory_entry> entries;
    try
    {
        for (const auto& e : fs::directory_iterator(
                dirPath, fs::directory_options::skip_permission_denied))
            entries.push_back(e);
    }
    catch (...) { return; }

    std::sort(entries.begin(), entries.end(),
        [](const fs::directory_entry& a, const fs::directory_entry& b) {
            bool aDir = a.is_directory();
            bool bDir = b.is_directory();
            if (aDir != bDir)
                return aDir > bDir;
            return a.path().filename() < b.path().filename();
        });

    for (const auto& e : entries)
    {
        std::string name = e.path().filename().string();
        if (name.empty() || name[0] == '.')
            continue;

        std::string fullPath = e.path().string();

        if (e.is_directory())
        {
            IETreeNode* node = parent->AddChild(name.c_str());
            m_nodePaths[node] = fullPath;
            BuildTree(node, fullPath);
        }
        else if (e.is_regular_file())
        {
            if (!m_filter.empty())
            {
                std::string ext = e.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(),
                    [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                std::string filter = m_filter;
                std::transform(filter.begin(), filter.end(), filter.begin(),
                    [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                if (ext != filter)
                    continue;
            }
            IETreeNode* node = parent->AddChild(name.c_str());
            m_nodePaths[node] = fullPath;
        }
    }
}

void IEFileBrowser::Update()
{
    m_treeView.Update();
}

void IEFileBrowser::Draw()
{
    m_treeView.Draw();
}
