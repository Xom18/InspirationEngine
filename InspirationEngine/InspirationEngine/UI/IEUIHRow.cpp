#include "../InspirationEngine.h"
#include "IEUIHRow.h"

int32_t IEUIHRow::Layout(int32_t x, int32_t y, int32_t w)
{
    SetRect(x, y, w, kRowH);
    int32_t count = static_cast<int32_t>(m_children.size());
    if (count == 0)
        return kRowH;

    int32_t colW = w / count;
    for (int32_t i = 0; i < count; ++i)
    {
        int32_t cx = x + i * colW;
        m_children[i]->SetRect(cx + kLabelW, y + 2, colW - kLabelW - 2, kRowH - 4);
    }
    return kRowH;
}

void IEUIHRow::Draw()
{
    IERenderer* r = GetRenderer();
    if (r == nullptr)
        return;

    SDL_Rect  rc  = GetRect();
    SDL_Color bg  = m_even ? kColRowBg : kColRowAlt;
    r->DrawRect(bg, rc.x, rc.y, rc.w, kRowH, SDL_BLENDMODE_NONE);

    IEFont*  font  = GetFont();
    int32_t  count = static_cast<int32_t>(m_children.size());
    int32_t  colW  = (count > 0) ? rc.w / count : rc.w;

    if (font != nullptr)
    {
        for (int32_t i = 0; i < static_cast<int32_t>(m_labels.size()); ++i)
        {
            int32_t lx = rc.x + i * colW;
            r->DrawText(font, m_labels[i].c_str(), kColLabel, lx, rc.y + 4);
        }
    }

    for (auto& c : m_children)
        c->Draw();
}

void IEUIHRow::DrawOverlay()
{
    for (auto& c : m_children)
        c->DrawOverlay();
}

void IEUIHRow::Update()
{
    for (auto& c : m_children)
        c->Update();
}
