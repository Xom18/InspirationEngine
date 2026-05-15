#include "../InspirationEngine.h"

void IELabel::Draw()
{
    IERenderer* r = GetRenderer();
    if (r == nullptr || m_font == nullptr)
        return;

    r->DrawText(m_font, m_text.c_str(), m_color, m_x, m_y);
}
