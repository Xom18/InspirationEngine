#include "../InspirationEngine.h"
#include "IESection.h"
#include "IERow.h"

// ─────────────────────────────────────────
// 자식 추가
// ─────────────────────────────────────────

IESection* IESection::AddSection(const std::string& title)
{
    auto sec = std::make_unique<IESection>();
    sec->SetTitle(title);
    IESection* ptr = sec.get();
    AddChild(std::move(sec));
    return ptr;
}

IETextBox* IESection::AddTextBox(const std::string& rowLabel)
{
    auto row = std::make_unique<IERow>();
    row->SetLabel(rowLabel);

    auto tb = std::make_unique<IETextBox>();
    tb->SetStyle(dTEXT_BOX_STYLE_EDITABLE);
    constexpr SDL_Color kTextCol = { 210, 210, 210, 255 };
    tb->SetDefaultColor(kTextCol);
    IETextBox* ptr = row->SetWidget(std::move(tb));

    AddChild(std::move(row));
    return ptr;
}

IESlider* IESection::AddSlider(const std::string& rowLabel, float minV, float maxV)
{
    auto row = std::make_unique<IERow>();
    row->SetLabel(rowLabel);

    auto sl = std::make_unique<IESlider>();
    sl->SetRange(minV, maxV);
    sl->SetShowValue(true);
    IESlider* ptr = row->SetWidget(std::move(sl));

    AddChild(std::move(row));
    return ptr;
}

IELabel* IESection::AddLabel(const std::string& text)
{
    auto row = std::make_unique<IERow>();
    row->SetFullWidth(true);

    auto lb = std::make_unique<IELabel>();
    lb->SetText(text.c_str());
    constexpr SDL_Color kTextCol = { 210, 210, 210, 255 };
    lb->SetColor(kTextCol);
    IELabel* ptr = row->SetWidget(std::move(lb));

    AddChild(std::move(row));
    return ptr;
}

IEDropdown* IESection::AddDropdown(const std::string& rowLabel, std::vector<std::string> items)
{
    auto row = std::make_unique<IERow>();
    row->SetLabel(rowLabel);

    auto dd = std::make_unique<IEDropdown>();
    dd->SetItems(std::move(items));
    IEDropdown* ptr = row->SetWidget(std::move(dd));

    AddChild(std::move(row));
    return ptr;
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

    int32_t cy     = y + kHeaderH + kPadY;
    int32_t cx     = x + kIndentX;
    int32_t cw     = w - kIndentX - kMarginR;
    int32_t rowIdx = 0;

    for (auto& child : m_children)
    {
        if (child->IsRow())
        {
            child->SetRowParity(rowIdx % 2 == 0);
            cy += child->Layout(cx, cy, cw) + kPadY;
            ++rowIdx;
        }
        else
        {
            // 중첩 섹션 — 전체 너비, rowIdx 미증가
            cy += child->Layout(x, cy, w) + kPadY;
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
    if (!m_collapsible || GetOwnerWindow() == nullptr)
        return false;

    float gxF = 0.0f, gyF = 0.0f;
    SDL_MouseButtonFlags btn = SDL_GetGlobalMouseState(&gxF, &gyF);
    bool lmb     = (btn & SDL_BUTTON_LMASK) != 0;
    bool clicked = lmb && !m_prevLMB;
    m_prevLMB    = lmb;

    if (!clicked)
        return false;

    int32_t winX = 0, winY = 0;
    SDL_GetWindowPosition(GetOwnerWindow()->GetSDLWindow(), &winX, &winY);
    int32_t mx = static_cast<int32_t>(gxF) - winX;
    int32_t my = static_cast<int32_t>(gyF) - winY;

    return mx >= m_headerRect.x && mx < m_headerRect.x + m_headerRect.w
        && my >= m_headerRect.y && my < m_headerRect.y + m_headerRect.h;
}

void IESection::Update()
{
    if (CheckHeaderClick())
        m_collapsed = !m_collapsed;

    if (m_collapsed)
        return;

    for (auto& c : m_children)
        c->Update();
}

// ─────────────────────────────────────────
// Draw
// ─────────────────────────────────────────

void IESection::Draw()
{
    IERenderer* r = GetRenderer();
    if (r == nullptr)
        return;

    // 헤더 배경
    SDL_Color hCol = m_collapsible ? kColHeader : kColHeaderH;
    r->DrawRect(hCol, m_headerRect.x, m_headerRect.y,
                m_headerRect.w, m_headerRect.h, SDL_BLENDMODE_NONE);

    if (GetFont() != nullptr)
    {
        int32_t tx = m_headerRect.x + 6;
        int32_t ty = m_headerRect.y + 4;

        if (m_collapsible)
        {
            r->DrawText(GetFont(), m_collapsed ? ">" : "v", kColLabel, tx, ty);
            tx += 14;
        }
        r->DrawText(GetFont(), m_title.c_str(), kColText, tx, ty);

        if (!m_headerLabel.empty())
        {
            int32_t lx = m_headerRect.x + static_cast<int32_t>(m_headerRect.w * kHeaderLabelRatio);
            r->DrawText(GetFont(), m_headerLabel.c_str(), m_headerLabelColor, lx, ty);
        }
    }

    if (m_collapsed)
        return;

    for (auto& c : m_children)
        c->Draw();
}

void IESection::DrawOverlay()
{
    if (m_collapsed)
        return;

    for (auto& c : m_children)
        c->DrawOverlay();
}
