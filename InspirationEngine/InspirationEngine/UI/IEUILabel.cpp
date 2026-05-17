#include "../InspirationEngine.h"

void IEUILabel::Draw()
{
    IERenderer* r = GetRenderer();
    IEFont* font = GetFont();
    if (r == nullptr || font == nullptr)
        return;

    SDL_Rect rect = GetRect();
    r->DrawText(font, m_text.c_str(), m_color, rect.x, rect.y);
}
