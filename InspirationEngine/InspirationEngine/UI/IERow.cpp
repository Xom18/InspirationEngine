#include "../InspirationEngine.h"
#include "IERow.h"

int32_t IERow::Layout(int32_t x, int32_t y, int32_t w)
{
    SetRect(x, y, w, kRowH);

    if (!m_children.empty())
    {
        if (m_fullWidth)
            m_children[0]->SetRect(x, y + 4, w, kRowH - 4);
        else
            m_children[0]->SetRect(x + kLabelW, y + 2, w - kLabelW, kRowH - 4);
    }

    return kRowH;
}

void IERow::Draw()
{
    IERenderer* r = GetRenderer();
    if (r == nullptr)
        return;

    SDL_Rect rc  = GetRect();
    SDL_Color bg = m_even ? kColRowBg : kColRowAlt;
    r->DrawRect(bg, rc.x, rc.y, rc.w, kRowH, SDL_BLENDMODE_NONE);

    if (GetFont() != nullptr && !m_label.empty() && !m_fullWidth)
        r->DrawText(GetFont(), m_label.c_str(), kColLabel, rc.x, rc.y + 4);

    for (auto& c : m_children)
        c->Draw();
}

void IERow::DrawOverlay()
{
    for (auto& c : m_children)
        c->DrawOverlay();
}

void IERow::Update()
{
    for (auto& c : m_children)
        c->Update();
}
