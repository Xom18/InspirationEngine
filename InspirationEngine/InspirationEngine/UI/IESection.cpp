#include "../InspirationEngine.h"
#include "IESection.h"

// ─────────────────────────────────────────
// 자식 추가
// ─────────────────────────────────────────

IESection* IESection::AddSection(const std::string& title)
{
    Entry e;
    e.kind    = EntryKind::Section;
    auto sec  = std::make_unique<IESection>();
    sec->SetTitle(title);
    IESection* ptr = sec.get();
    e.section = std::move(sec);
    m_entries.push_back(std::move(e));
    return ptr;
}

IETextBox* IESection::AddTextBox(const std::string& rowLabel)
{
    Entry e;
    e.kind     = EntryKind::TextBox;
    e.rowLabel = rowLabel;
    auto tb    = std::make_unique<IETextBox>();
    tb->SetStyle(dTEXT_BOX_STYLE_EDITABLE);
    constexpr SDL_Color kTextCol = { 210, 210, 210, 255 };
    tb->SetDefaultColor(kTextCol);
    IETextBox* ptr = tb.get();
    e.textBox  = std::move(tb);
    m_entries.push_back(std::move(e));
    return ptr;
}

IESlider* IESection::AddSlider(const std::string& rowLabel, float minV, float maxV)
{
    Entry e;
    e.kind     = EntryKind::Slider;
    e.rowLabel = rowLabel;
    auto sl    = std::make_unique<IESlider>();
    sl->SetRange(minV, maxV);
    sl->SetShowValue(true);
    IESlider* ptr = sl.get();
    e.slider   = std::move(sl);
    m_entries.push_back(std::move(e));
    return ptr;
}

IELabel* IESection::AddLabel(const std::string& text)
{
    Entry e;
    e.kind  = EntryKind::Label;
    auto lb = std::make_unique<IELabel>();
    lb->SetText(text.c_str());
    lb->SetColor(kColText);
    IELabel* ptr = lb.get();
    e.label  = std::move(lb);
    m_entries.push_back(std::move(e));
    return ptr;
}

// ─────────────────────────────────────────
// 컨텍스트 전파
// ─────────────────────────────────────────

void IESection::PropagateToEntry(Entry& e)
{
    switch (e.kind)
    {
    case EntryKind::TextBox:
        if (e.textBox)
        {
            e.textBox->SetFont(m_font);
            e.textBox->SetOwnerWindow(m_ownerWindow);
            e.textBox->SetRenderer(m_renderer);
        }
        break;
    case EntryKind::Slider:
        if (e.slider)
        {
            e.slider->SetFont(m_font);
            e.slider->SetOwnerWindow(m_ownerWindow);
            e.slider->SetRenderer(m_renderer);
        }
        break;
    case EntryKind::Label:
        if (e.label)
        {
            e.label->SetFont(m_font);
            e.label->SetOwnerWindow(m_ownerWindow);
            e.label->SetRenderer(m_renderer);
        }
        break;
    case EntryKind::Section:
        if (e.section)
            e.section->SetFont(m_font),
            e.section->SetOwnerWindow(m_ownerWindow),
            e.section->SetRenderer(m_renderer);
        break;
    }
}

void IESection::SetFont(IEFont* f)
{
    m_font = f;
    for (auto& e : m_entries) PropagateToEntry(e);
}

void IESection::SetOwnerWindow(IEWindow* w)
{
    m_ownerWindow = w;
    for (auto& e : m_entries) PropagateToEntry(e);
}

void IESection::SetRenderer(IERenderer* r)
{
    m_renderer = r;
    for (auto& e : m_entries) PropagateToEntry(e);
}

// ─────────────────────────────────────────
// Layout
// ─────────────────────────────────────────

int32_t IESection::Layout(int32_t x, int32_t y, int32_t w)
{
    m_x = x; m_y = y; m_w = w;
    m_headerRect = { x, y, w, kHeaderH };

    if (m_collapsed)
    {
        m_totalH = kHeaderH;
        return m_totalH;
    }

    int32_t cy = y + kHeaderH + kPadY;
    int32_t cx = x + kIndentX;
    int32_t cw = w - kIndentX - kMarginR;

    for (auto& e : m_entries)
    {
        switch (e.kind)
        {
        case EntryKind::TextBox:
        {
            e.labelRect  = { cx, cy + 4, kLabelW, kRowH - 4 };
            e.widgetRect = { cx + kLabelW, cy + 2, cw - kLabelW, kRowH - 4 };
            if (e.textBox)
                e.textBox->SetRect(e.widgetRect.x, e.widgetRect.y,
                                   e.widgetRect.w, e.widgetRect.h);
            cy += kRowH + kPadY;
            break;
        }
        case EntryKind::Slider:
        {
            e.labelRect  = { cx, cy + 4, kLabelW, kRowH - 4 };
            e.widgetRect = { cx + kLabelW, cy + 2, cw - kLabelW, kRowH - 4 };
            if (e.slider)
                e.slider->SetRect(e.widgetRect.x, e.widgetRect.y,
                                  e.widgetRect.w, e.widgetRect.h);
            cy += kRowH + kPadY;
            break;
        }
        case EntryKind::Label:
        {
            e.labelRect  = {};
            e.widgetRect = { cx, cy + 4, cw, kRowH - 4 };
            if (e.label)
                e.label->SetRect(e.widgetRect.x, e.widgetRect.y, 0, 0);
            cy += kRowH + kPadY;
            break;
        }
        case EntryKind::Section:
        {
            if (e.section)
            {
                int32_t childH = e.section->Layout(x, cy, w);
                cy += childH + kPadY;
            }
            break;
        }
        }
    }

    m_totalH = cy - y;
    return m_totalH;
}

// ─────────────────────────────────────────
// Update
// ─────────────────────────────────────────

bool IESection::CheckHeaderClick()
{
    if (!m_collapsible || m_ownerWindow == nullptr)
        return false;

    float gxF = 0.0f, gyF = 0.0f;
    SDL_MouseButtonFlags btn = SDL_GetGlobalMouseState(&gxF, &gyF);
    bool lmb = (btn & SDL_BUTTON_LMASK) != 0;
    bool clicked = lmb && !m_prevLMB;
    m_prevLMB = lmb;

    if (!clicked)
        return false;

    int32_t winX = 0, winY = 0;
    SDL_GetWindowPosition(m_ownerWindow->GetSDLWindow(), &winX, &winY);
    int32_t mx = static_cast<int32_t>(gxF) - winX;
    int32_t my = static_cast<int32_t>(gyF) - winY;

    bool inHeader = mx >= m_headerRect.x && mx < m_headerRect.x + m_headerRect.w
                 && my >= m_headerRect.y && my < m_headerRect.y + m_headerRect.h;
    return inHeader;
}

void IESection::Update()
{
    if (CheckHeaderClick())
        m_collapsed = !m_collapsed;

    if (m_collapsed)
        return;

    for (auto& e : m_entries)
    {
        switch (e.kind)
        {
        case EntryKind::Slider:
            if (e.slider) e.slider->Update();
            break;
        case EntryKind::Label:
            break;
        case EntryKind::TextBox:
            // TextBox 키보드 입력은 IECore::GetFocusedTextBox() 가 관리
            if (e.textBox) e.textBox->Update();
            break;
        case EntryKind::Section:
            if (e.section) e.section->Update();
            break;
        }
    }
}

// ─────────────────────────────────────────
// Draw
// ─────────────────────────────────────────

void IESection::Draw(IERenderer* r)
{
    if (r == nullptr)
        return;

    // 헤더
    SDL_Color hCol = m_collapsible ? kColHeader : kColHeaderH;
    r->DrawRect(hCol, m_headerRect.x, m_headerRect.y,
                m_headerRect.w, m_headerRect.h, SDL_BLENDMODE_NONE);

    if (m_font != nullptr)
    {
        const char* chevron = m_collapsed ? ">" : "v";
        int32_t tx = m_headerRect.x + 6;
        int32_t ty = m_headerRect.y + 4;
        if (m_collapsible)
        {
            r->DrawText(m_font, chevron, kColLabel, tx, ty);
            tx += 14;
        }
        r->DrawText(m_font, m_title.c_str(), kColText, tx, ty);
    }

    if (m_collapsed)
        return;

    // 행 렌더링
    int32_t rowIdx = 0;
    for (auto& e : m_entries)
    {
        switch (e.kind)
        {
        case EntryKind::TextBox:
        {
            // 행 배경
            SDL_Color rowBg = (rowIdx % 2 == 0) ? kColRowBg : kColRowAlt;
            r->DrawRect(rowBg, m_x + kIndentX, e.widgetRect.y - 2,
                        m_w - kIndentX, kRowH, SDL_BLENDMODE_NONE);

            // 레이블
            if (m_font != nullptr && !e.rowLabel.empty())
                r->DrawText(m_font, e.rowLabel.c_str(), kColLabel,
                            e.labelRect.x, e.labelRect.y);

            // 텍스트박스 배경 + 포커스 테두리
            if (e.textBox)
            {
                bool focused = (IECore::GetFocusedTextBox() == e.textBox.get());
                SDL_Color bor = focused ? kColTbFoc : kColTbBor;
                SDL_Rect& wr  = e.widgetRect;
                r->DrawRect(kColTbBg, wr.x, wr.y, wr.w, wr.h, SDL_BLENDMODE_NONE);
                r->DrawLine(bor, wr.x,        wr.y,        wr.x + wr.w, wr.y);
                r->DrawLine(bor, wr.x,        wr.y + wr.h, wr.x + wr.w, wr.y + wr.h);
                r->DrawLine(bor, wr.x,        wr.y,        wr.x,        wr.y + wr.h);
                r->DrawLine(bor, wr.x + wr.w, wr.y,        wr.x + wr.w, wr.y + wr.h);
                e.textBox->Draw();
            }
            ++rowIdx;
            break;
        }
        case EntryKind::Slider:
        {
            SDL_Color rowBg = (rowIdx % 2 == 0) ? kColRowBg : kColRowAlt;
            r->DrawRect(rowBg, m_x + kIndentX, e.widgetRect.y - 2,
                        m_w - kIndentX, kRowH, SDL_BLENDMODE_NONE);

            if (m_font != nullptr && !e.rowLabel.empty())
                r->DrawText(m_font, e.rowLabel.c_str(), kColLabel,
                            e.labelRect.x, e.labelRect.y);

            if (e.slider) e.slider->Draw();
            ++rowIdx;
            break;
        }
        case EntryKind::Label:
        {
            SDL_Color rowBg = (rowIdx % 2 == 0) ? kColRowBg : kColRowAlt;
            r->DrawRect(rowBg, m_x + kIndentX, e.widgetRect.y - 2,
                        m_w - kIndentX, kRowH, SDL_BLENDMODE_NONE);
            if (e.label) e.label->Draw();
            ++rowIdx;
            break;
        }
        case EntryKind::Section:
        {
            if (e.section) e.section->Draw(r);
            break;
        }
        }
    }
}
