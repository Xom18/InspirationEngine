#include "../InspirationEngine.h"

IETreeNode* IEUITreeView::AddRootNode(const char* label)
{
    m_roots.push_back(std::make_unique<IETreeNode>(label));
    return m_roots.back().get();
}

void IEUITreeView::Clear()
{
    m_roots.clear();
    m_selectedNode = nullptr;
    m_scroll.ResetScroll();
}

void IEUITreeView::BuildFlatListRecursive(IETreeNode* node, int32_t depth, int32_t& y, std::vector<FlatEntry>& flat) const
{
    flat.push_back({ node, depth, y });
    y += kRowH;
    if (node->IsExpanded())
        for (const auto& child : node->GetChildren())
            BuildFlatListRecursive(child.get(), depth + 1, y, flat);
}

void IEUITreeView::BuildFlatList(std::vector<FlatEntry>& flat) const
{
    int32_t y = 0;
    for (const auto& root : m_roots)
        BuildFlatListRecursive(root.get(), 0, y, flat);
}

void IEUITreeView::DrawArrow(IERenderer* r, int32_t x, int32_t y, bool expanded) const
{
    SDL_Color col = { 180, 180, 180, 255 };
    int32_t cx = x + kArrowW / 2;
    int32_t cy = y + kRowH / 2;
    constexpr int32_t s = 4;

    if (expanded)
    {
        r->DrawLine(col, cx - s, cy - s / 2, cx + s, cy - s / 2);
        r->DrawLine(col, cx + s, cy - s / 2, cx,     cy + s);
        r->DrawLine(col, cx,     cy + s,     cx - s, cy - s / 2);
    }
    else
    {
        r->DrawLine(col, cx - s / 2, cy - s, cx - s / 2, cy + s);
        r->DrawLine(col, cx - s / 2, cy - s, cx + s,     cy);
        r->DrawLine(col, cx + s,     cy,     cx - s / 2, cy + s);
    }
}

void IEUITreeView::Update()
{
    m_scroll.Update();

    IEWindow* ownerWindow = GetOwnerWindow();
    if (ownerWindow == nullptr || IECore::GetMouseOnWindow() != ownerWindow)
    {
        m_prevLMB = false;
        return;
    }

    float gx = 0.0f, gy = 0.0f;
    SDL_MouseButtonFlags btn = SDL_GetGlobalMouseState(&gx, &gy);

    int32_t winX = 0, winY = 0;
    SDL_GetWindowPosition(ownerWindow->GetSDLWindow(), &winX, &winY);

    int32_t mx = static_cast<int32_t>(gx) - winX;
    int32_t my = static_cast<int32_t>(gy) - winY;

    bool lmb     = (btn & SDL_BUTTON_LMASK) != 0;
    bool clicked = lmb && !m_prevLMB;
    m_prevLMB = lmb;

    if (!clicked)
        return;

    SDL_Rect rect = GetRect();
    if (mx < rect.x || mx >= rect.x + rect.w ||
        my < rect.y || my >= rect.y + rect.h)
        return;

    std::vector<FlatEntry> flat;
    BuildFlatList(flat);

    int32_t scrollY = m_scroll.GetScrollOffsetY();

    for (auto& entry : flat)
    {
        int32_t rowY = rect.y + entry.contentY - scrollY;
        if (my < rowY || my >= rowY + kRowH)
            continue;

        int32_t indentX = rect.x + kPadX + entry.depth * kIndentW;

        if (entry.node->HasChildren() && mx >= indentX && mx < indentX + kArrowW)
        {
            entry.node->SetExpanded(!entry.node->IsExpanded());
            std::vector<FlatEntry> rebuilt;
            BuildFlatList(rebuilt);
            m_scroll.SetContentHeight(static_cast<int32_t>(rebuilt.size()) * kRowH);
            return;
        }

        int32_t labelX = indentX + kArrowW + 2;
        if (mx >= labelX)
        {
            if (m_selectedNode != nullptr)
                m_selectedNode->SetSelected(false);
            m_selectedNode = entry.node;
            entry.node->SetSelected(true);
            if (m_callback)
                m_callback(entry.node);
        }
        return;
    }
}

void IEUITreeView::Draw()
{
    IERenderer* r = GetRenderer();
    IEFont* font = GetFont();
    if (r == nullptr || font == nullptr)
        return;

    std::vector<FlatEntry> flat;
    BuildFlatList(flat);

    m_scroll.SetContentHeight(static_cast<int32_t>(flat.size()) * kRowH);

    int32_t scrollY = m_scroll.GetScrollOffsetY();

    m_scroll.BeginDraw();

    SDL_Rect rect = GetRect();

    for (const auto& entry : flat)
    {
        int32_t rowY = rect.y + entry.contentY - scrollY;

        if (rowY + kRowH <= rect.y || rowY >= rect.y + rect.h)
            continue;

        int32_t indentX = rect.x + kPadX + entry.depth * kIndentW;

        if (entry.node->IsSelected())
            r->DrawRect({ 70, 130, 180, 120 }, rect.x, rowY, rect.w, kRowH, SDL_BLENDMODE_BLEND);

        if (entry.node->HasChildren())
            DrawArrow(r, indentX, rowY, entry.node->IsExpanded());

        int32_t labelX = indentX + kArrowW + 2;
        int32_t labelY = rowY + (kRowH - font->GetHeight()) / 2;
        r->DrawText(font, entry.node->GetLabel().c_str(), { 220, 220, 220, 255 }, labelX, labelY);
    }

    m_scroll.EndDraw();
    m_scroll.Draw();
}
