#include "../InspirationEngine.h"

// ── z-order 오버레이 ─────────────────────────────────────────

void IEDropdown::DrawOverlay()
{
    if (m_isOpen)
        Draw();
}

// ── 설정 전파 ────────────────────────────────────────────────

void IEDropdown::SetRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    IEUIBase::SetRect(x, y, w, h);
    UpdateListRect();
}

void IEDropdown::UpdateListRect()
{
    SDL_Rect rect    = GetRect();
    int32_t  count   = static_cast<int32_t>(m_items.size());
    int32_t  visible = std::min(count, kMaxVisible);
    m_listRect = { rect.x, rect.y + rect.h, rect.w, visible * kRowH };
}

// ── 항목 관리 ────────────────────────────────────────────────

void IEDropdown::SetItems(std::vector<std::string> items)
{
    m_items         = std::move(items);
    m_selectedIndex = -1;
    m_hoveredIndex  = -1;
    m_scrollOffset  = 0;
    m_isOpen        = false;
    UpdateListRect();
}

void IEDropdown::AddItem(const std::string& item)
{
    m_items.push_back(item);
    UpdateListRect();
}

void IEDropdown::ClearItems()
{
    m_items.clear();
    m_selectedIndex = -1;
    m_hoveredIndex  = -1;
    m_scrollOffset  = 0;
    m_isOpen        = false;
    UpdateListRect();
}

void IEDropdown::SetSelectedIndex(int32_t index)
{
    int32_t count   = static_cast<int32_t>(m_items.size());
    m_selectedIndex = (index >= 0 && index < count) ? index : -1;
}

const std::string* IEDropdown::GetSelectedItem() const
{
    if (m_selectedIndex < 0 || m_selectedIndex >= static_cast<int32_t>(m_items.size()))
        return nullptr;
    return &m_items[m_selectedIndex];
}

// ── Update ───────────────────────────────────────────────────

void IEDropdown::Update()
{
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

    SDL_Rect rect     = GetRect();
    bool     onHeader = (mx >= rect.x && mx < rect.x + rect.w &&
                         my >= rect.y && my < rect.y + rect.h);

    if (m_isOpen)
    {
        // 스크롤 (마우스 휠)
        bool onList = (mx >= m_listRect.x && mx < m_listRect.x + m_listRect.w &&
                       my >= m_listRect.y && my < m_listRect.y + m_listRect.h);
        if (onList)
        {
            float wheel = IECore::GetInput().GetMouseWheelY();
            if (wheel != 0.0f)
            {
                int32_t count      = static_cast<int32_t>(m_items.size());
                int32_t maxOffset  = std::max(0, count - kMaxVisible);
                m_scrollOffset     = std::clamp(
                    m_scrollOffset - static_cast<int32_t>(wheel), 0, maxOffset);
            }

            // 호버
            int32_t localY  = my - m_listRect.y;
            int32_t rowIdx  = localY / kRowH + m_scrollOffset;
            int32_t count   = static_cast<int32_t>(m_items.size());
            m_hoveredIndex  = (rowIdx >= 0 && rowIdx < count) ? rowIdx : -1;

            if (clicked && m_hoveredIndex >= 0)
            {
                if (m_hoveredIndex != m_selectedIndex)
                {
                    m_selectedIndex = m_hoveredIndex;
                    if (m_callback)
                        m_callback(m_selectedIndex);
                }
                m_isOpen       = false;
                m_hoveredIndex = -1;
            }
        }
        else if (clicked)
        {
            // 목록 밖 클릭 → 닫기 (헤더 클릭 토글 포함)
            m_isOpen       = false;
            m_hoveredIndex = -1;
        }
    }
    else
    {
        m_hoveredIndex = -1;
        if (clicked && onHeader)
            m_isOpen = true;
    }
}

// ── Draw ─────────────────────────────────────────────────────

void IEDropdown::DrawArrow(IERenderer* r, int32_t cx, int32_t cy) const
{
    // 간단한 삼각형 화살표 (선 3개)
    if (m_isOpen)
    {
        // ▲
        r->DrawLine(kColArrow, cx - 5, cy + 3, cx,     cy - 3);
        r->DrawLine(kColArrow, cx,     cy - 3, cx + 5, cy + 3);
        r->DrawLine(kColArrow, cx - 5, cy + 3, cx + 5, cy + 3);
    }
    else
    {
        // ▼
        r->DrawLine(kColArrow, cx - 5, cy - 3, cx,     cy + 3);
        r->DrawLine(kColArrow, cx,     cy + 3, cx + 5, cy - 3);
        r->DrawLine(kColArrow, cx - 5, cy - 3, cx + 5, cy - 3);
    }
}

void IEDropdown::Draw()
{
    IERenderer* r = GetRenderer();
    if (r == nullptr)
        return;

    SDL_Rect rect   = GetRect();
    IEFont*  font   = GetFont();
    int32_t  headerH = rect.h;

    // 헤더 배경
    r->DrawRect(kColBg, rect.x, rect.y, rect.w, headerH, SDL_BLENDMODE_NONE);
    r->DrawLine(kColBorder, rect.x, rect.y,           rect.x + rect.w, rect.y);
    r->DrawLine(kColBorder, rect.x, rect.y + headerH, rect.x + rect.w, rect.y + headerH);
    r->DrawLine(kColBorder, rect.x,           rect.y, rect.x,           rect.y + headerH);
    r->DrawLine(kColBorder, rect.x + rect.w,  rect.y, rect.x + rect.w,  rect.y + headerH);

    // 선택 항목 텍스트
    if (font != nullptr)
    {
        const char* label = (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int32_t>(m_items.size()))
            ? m_items[m_selectedIndex].c_str()
            : "";
        int32_t textY = rect.y + (headerH - font->GetHeight()) / 2;
        r->DrawText(font, label, kColText, rect.x + 6, textY);
    }

    // 화살표
    int32_t arrowCx = rect.x + rect.w - kArrowW / 2;
    int32_t arrowCy = rect.y + headerH / 2;
    DrawArrow(r, arrowCx, arrowCy);

    if (!m_isOpen || m_items.empty())
        return;

    // 펼쳐진 목록
    r->DrawRect(kColListBg, m_listRect.x, m_listRect.y,
                m_listRect.w, m_listRect.h, SDL_BLENDMODE_NONE);
    r->DrawLine(kColBorder, m_listRect.x, m_listRect.y + m_listRect.h,
                m_listRect.x + m_listRect.w, m_listRect.y + m_listRect.h);
    r->DrawLine(kColBorder, m_listRect.x,            m_listRect.y,
                m_listRect.x,            m_listRect.y + m_listRect.h);
    r->DrawLine(kColBorder, m_listRect.x + m_listRect.w, m_listRect.y,
                m_listRect.x + m_listRect.w, m_listRect.y + m_listRect.h);

    int32_t count   = static_cast<int32_t>(m_items.size());
    int32_t visible = std::min(count, kMaxVisible);

    for (int32_t i = 0; i < visible; ++i)
    {
        int32_t itemIdx = i + m_scrollOffset;
        if (itemIdx >= count)
            break;

        int32_t rowY = m_listRect.y + i * kRowH;

        SDL_Color rowCol;
        if (itemIdx == m_selectedIndex)
            rowCol = kColSel;
        else if (itemIdx == m_hoveredIndex)
            rowCol = kColHover;
        else
            rowCol = kColRow;

        r->DrawRect(rowCol, m_listRect.x, rowY, m_listRect.w, kRowH, SDL_BLENDMODE_NONE);

        if (font != nullptr)
            r->DrawText(font, m_items[itemIdx].c_str(), kColText, m_listRect.x + 6, rowY + 4);
    }
}
