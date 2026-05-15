#pragma once
#include <functional>
#include <vector>
#include <memory>

class IEWindow;
class IETreeNode;

class IETreeView : public IEUIBase
{
public:
    virtual void Draw() override;
    virtual void Update() override;

    /// <summary>
    /// 렌더러 설정 — 내부 IEScrollView에도 전파
    /// </summary>
    void SetRenderer(IERenderer* r)
    {
        IEUIBase::SetRenderer(r);
        m_scroll.SetRenderer(r);
    }

    /// <summary>
    /// 뷰포트 영역 설정
    /// </summary>
    void SetRect(int32_t x, int32_t y, int32_t w, int32_t h)
    {
        m_rect = { x, y, w, h };
        m_scroll.SetRect(x, y, w, h);
    }

    /// <summary>
    /// 폰트 설정
    /// </summary>
    void SetFont(IEFont* font) { m_font = font; }

    /// <summary>
    /// 마우스 이벤트 소유 창 설정
    /// </summary>
    void SetOwnerWindow(IEWindow* window)
    {
        m_ownerWindow = window;
        m_scroll.SetOwnerWindow(window);
    }

    /// <summary>
    /// 노드 선택 시 호출되는 콜백 설정
    /// </summary>
    void SetCallback(std::function<void(IETreeNode*)> callback)
    {
        m_callback = std::move(callback);
    }

    /// <summary>
    /// 루트 노드 추가 — 추가된 노드 포인터 반환
    /// </summary>
    IETreeNode* AddRootNode(const char* label);

    /// <summary>
    /// 모든 노드 제거
    /// </summary>
    void Clear();

    /// <summary>
    /// 현재 선택된 노드 반환 — 없으면 nullptr
    /// </summary>
    IETreeNode* GetSelectedNode() const { return m_selectedNode; }

private:
    struct FlatEntry
    {
        IETreeNode* node;
        int32_t     depth;
        int32_t     contentY;
    };

    void BuildFlatList(std::vector<FlatEntry>& flat) const;
    void BuildFlatListRecursive(IETreeNode* node, int32_t depth, int32_t& y, std::vector<FlatEntry>& flat) const;
    void DrawArrow(IERenderer* r, int32_t x, int32_t y, bool expanded) const;

    static constexpr int32_t kRowH    = 24;
    static constexpr int32_t kIndentW = 16;
    static constexpr int32_t kArrowW  = 14;
    static constexpr int32_t kPadX    = 4;

    IEFont*      m_font        = nullptr;
    IEWindow*    m_ownerWindow = nullptr;
    SDL_Rect     m_rect        = {};
    IEScrollView m_scroll;

    std::vector<std::unique_ptr<IETreeNode>> m_roots;
    IETreeNode*                              m_selectedNode = nullptr;
    std::function<void(IETreeNode*)>         m_callback;
    bool                                     m_prevLMB = false;
};
