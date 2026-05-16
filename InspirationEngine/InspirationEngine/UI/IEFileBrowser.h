#pragma once
#include <functional>
#include <string>
#include <unordered_map>

class IEFileBrowser : public IEUIBase
{
public:
    IEFileBrowser();

    virtual void Draw() override;
    virtual void Update() override;

    virtual void SetRenderer(IERenderer* r) override;
    virtual void SetRect(int32_t x, int32_t y, int32_t w, int32_t h) override;
    virtual void SetFont(IEFont* font) override;
    virtual void SetOwnerWindow(IEWindow* window) override;

    /// <summary>
    /// 탐색할 루트 디렉터리 경로 설정 — 설정 즉시 Refresh 호출
    /// </summary>
    void SetRootPath(const char* path);

    /// <summary>
    /// 표시할 파일 확장자 필터 (예: ".png") — 빈 문자열이면 전체 표시
    /// </summary>
    void SetFilter(const char* ext);

    /// <summary>
    /// 파일 선택 시 호출되는 콜백 설정 (전체 경로 전달)
    /// </summary>
    void SetCallback(std::function<void(const std::string&)> callback);

    /// <summary>
    /// 현재 루트 경로 기준으로 트리 재구성
    /// </summary>
    void Refresh();

    const std::string& GetSelectedPath() const { return m_selectedPath; }

private:
    void BuildTree(IETreeNode* parent, const std::string& dirPath);

    IETreeView  m_treeView;
    std::string m_rootPath;
    std::string m_filter;
    std::string m_selectedPath;
    std::unordered_map<IETreeNode*, std::string> m_nodePaths;
    std::function<void(const std::string&)> m_callback;
};
