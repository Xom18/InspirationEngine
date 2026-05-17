#include "../InspirationEngine.h"
#include "IEUIBase.h"

void IEUIBase::SetFont(IEFont* font)
{
    m_font = font;
    for (auto& c : m_children) c->SetFont(font);
}

void IEUIBase::SetRenderer(IERenderer* renderer)
{
    m_renderer = renderer;
    for (auto& c : m_children) c->SetRenderer(renderer);
}

void IEUIBase::SetOwnerWindow(IEWindow* win)
{
    m_ownerWindow = win;
    for (auto& c : m_children) c->SetOwnerWindow(win);
}

void IEUIBase::DrawOverlay()
{
    for (auto& c : m_children) c->DrawOverlay();
}
