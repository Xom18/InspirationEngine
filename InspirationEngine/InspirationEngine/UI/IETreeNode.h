#pragma once
#include <vector>
#include <string>
#include <memory>

class IETreeNode
{
public:
    explicit IETreeNode(const char* label) : m_label(label) {}

    /// <summary>
    /// 자식 노드 추가 — 추가된 노드 포인터 반환
    /// </summary>
    IETreeNode* AddChild(const char* label)
    {
        m_children.push_back(std::make_unique<IETreeNode>(label));
        return m_children.back().get();
    }

    /// <summary>
    /// 모든 자식 노드 제거
    /// </summary>
    void ClearChildren()
    {
        m_children.clear();
    }

    const std::string& GetLabel() const { return m_label; }
    void SetLabel(const char* label)    { m_label = label; }

    bool IsExpanded() const      { return m_expanded; }
    void SetExpanded(bool v)     { m_expanded = v; }

    bool IsSelected() const      { return m_selected; }
    void SetSelected(bool v)     { m_selected = v; }

    /// <summary>
    /// 파일 경로 등 임의 데이터 연결
    /// </summary>
    void  SetUserData(void* data) { m_userData = data; }
    void* GetUserData() const     { return m_userData; }

    bool HasChildren() const { return !m_children.empty(); }

    const std::vector<std::unique_ptr<IETreeNode>>& GetChildren() const { return m_children; }

private:
    std::string m_label;
    std::vector<std::unique_ptr<IETreeNode>> m_children;
    bool  m_expanded = false;
    bool  m_selected = false;
    void* m_userData = nullptr;
};
