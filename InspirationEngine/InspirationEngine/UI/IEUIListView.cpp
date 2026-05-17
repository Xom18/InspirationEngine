#include "../InspirationEngine.h"

// ── 설정 전파 ────────────────────────────────────────────────

void IEUIListView::SetRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    IEUIBase::SetRect(x, y, w, h);
    m_scroll.SetRect(x, y, w, h);
    UpdateScrollHeight();
}

void IEUIListView::SetOwnerWindow(IEWindow* w)
{
    IEUIBase::SetOwnerWindow(w);
    m_scroll.SetOwnerWindow(w);
}

void IEUIListView::SetRenderer(IERenderer* r)
{
    IEUIBase::SetRenderer(r);
    m_scroll.SetRenderer(r);
}

// ── 항목 관리 ────────────────────────────────────────────────

void IEUIListView::UpdateScrollHeight()
{
    m_scroll.SetContentHeight(static_cast<int32_t>(m_items.size()) * kRowH);
}

void IEUIListView::SetItems(std::vector<std::string> items)
{
    m_items         = std::move(items);
    m_selectedIndex = -1;
    m_hoveredIndex  = -1;
    m_scroll.ResetScroll();
    UpdateScrollHeight();
}

void IEUIListView::AddItem(const std::string& item)
{
    m_items.push_back(item);
    UpdateScrollHeight();
}

void IEUIListView::ClearItems()
{
    m_items.clear();
    m_selectedIndex = -1;
    m_hoveredIndex  = -1;
    m_scroll.ResetScroll();
    UpdateScrollHeight();
}

void IEUIListView::RefreshItems(std::vector<std::string> items)
{
    m_items = std::move(items);
    int32_t count = static_cast<int32_t>(m_items.size());
    if (m_selectedIndex >= count)
        m_selectedIndex = -1;
    if (m_hoveredIndex >= count)
        m_hoveredIndex = -1;
    UpdateScrollHeight();
}

void IEUIListView::SetSelectedIndex(int32_t index)
{
    int32_t count = static_cast<int32_t>(m_items.size());
    m_selectedIndex = (index >= 0 && index < count) ? index : -1;
}

const std::string* IEUIListView::GetSelectedItem() const
{
    if (m_selectedIndex < 0 || m_selectedIndex >= static_cast<int32_t>(m_items.size()))
        return nullptr;
    return &m_items[m_selectedIndex];
}

// ── Update ───────────────────────────────────────────────────

void IEUIListView::Update()
{
    m_scroll.Update();

    IEWindow* ownerWindow = GetOwnerWindow();
    if (ownerWindow == nullptr || IECore::GetMouseOnWindow() != ownerWindow)
    {
        m_prevLMB      = false;
        m_hoveredIndex = -1;
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
    m_prevLMB    = lmb;

    SDL_Rect rect    = GetRect();
    bool     onList  = (mx >= rect.x && mx < rect.x + rect.w &&
                        my >= rect.y && my < rect.y + rect.h);

    if (!onList)
    {
        m_hoveredIndex = -1;
        return;
    }

    int32_t scrollY    = m_scroll.GetScrollOffsetY();
    int32_t localY     = my - rect.y + scrollY;
    int32_t rowIndex   = localY / kRowH;
    int32_t count      = static_cast<int32_t>(m_items.size());
    m_hoveredIndex     = (rowIndex >= 0 && rowIndex < count) ? rowIndex : -1;

    if (clicked && m_hoveredIndex >= 0 && m_hoveredIndex != m_selectedIndex)
    {
        m_selectedIndex = m_hoveredIndex;
        if (m_callback)
            m_callback(m_selectedIndex);
    }
}

// ── Draw ─────────────────────────────────────────────────────

void IEUIListView::Draw()
{
    IERenderer* r = GetRenderer();
    if (r == nullptr)
        return;

    SDL_Rect rect    = GetRect();
    IEFont*  font    = GetFont();
    int32_t  scrollY = m_scroll.GetScrollOffsetY();
    int32_t  count   = static_cast<int32_t>(m_items.size());

    r->DrawRect(kColBg, rect.x, rect.y, rect.w, rect.h, SDL_BLENDMODE_NONE);

    m_scroll.BeginDraw();

    int32_t firstRow = scrollY / kRowH;
    int32_t lastRow  = std::min(count, firstRow + rect.h / kRowH + 2);

    for (int32_t i = firstRow; i < lastRow; ++i)
    {
        int32_t rowY = rect.y + i * kRowH - scrollY;
        if (rowY + kRowH < rect.y || rowY >= rect.y + rect.h)
            continue;

        SDL_Color rowCol;
        if (i == m_selectedIndex)
            rowCol = kColSel;
        else if (i == m_hoveredIndex)
            rowCol = kColHover;
        else
            rowCol = (i % 2 == 0) ? kColRow : kColRowAlt;

        r->DrawRect(rowCol, rect.x, rowY, rect.w, kRowH, SDL_BLENDMODE_NONE);

        if (font != nullptr)
            r->DrawText(font, m_items[i].c_str(), kColText, rect.x + 6, rowY + 4);
    }

    m_scroll.EndDraw();
    m_scroll.Draw();
}
