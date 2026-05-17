#include "../InspirationEngine/InspirationEngine.h"
#include "IEHierarchy.h"
#include <cstdio>

void IEHierarchy::SetFont(IEFont* f)
{
    IEPanel::SetFont(f);
    m_list.SetFont(f);
}

void IEHierarchy::SetOwnerWindow(IEWindow* w)
{
    IEPanel::SetOwnerWindow(w);
    m_list.SetOwnerWindow(w);
}

void IEHierarchy::SetRenderer(IERenderer* r)
{
    IEPanel::SetRenderer(r);
    m_list.SetRenderer(r);
}

void IEHierarchy::SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    m_list.SetRect(x, y, w, h);
}

void IEHierarchy::Update(float /*dt*/)
{
    if (m_scene != nullptr)
    {
        const auto& objs = m_scene->GetObjects();
        std::vector<std::string> items;
        items.reserve(objs.size());

        char buf[64];
        for (int32_t i = 0; i < static_cast<int32_t>(objs.size()); ++i)
        {
            const std::string& name = objs[i]->GetName();
            if (!name.empty())
            {
                std::snprintf(buf, sizeof(buf), "%s", name.c_str());
            }
            else
            {
                auto* t = objs[i]->GetComponent<IETransformComponent>();
                if (t != nullptr)
                    std::snprintf(buf, sizeof(buf), "Object %d  (%.0f, %.0f)", i, t->GetX(), t->GetY());
                else
                    std::snprintf(buf, sizeof(buf), "Object %d", i);
            }
            items.emplace_back(buf);
        }
        m_list.RefreshItems(std::move(items));
    }

    m_list.Update();
}

void IEHierarchy::Draw(IERenderer* /*r*/)
{
    m_list.Draw();
}
