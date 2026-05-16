#include "../InspirationEngine/InspirationEngine.h"
#include "IEInspectorPanel.h"
#include <cstdio>
#include <cstring>

// ─────────────────────────────────────────
// Init
// ─────────────────────────────────────────

IEInspectorPanel::IEInspectorPanel()
{
    m_tbX.SetStyle(dTEXT_BOX_STYLE_EDITABLE);
    m_tbY.SetStyle(dTEXT_BOX_STYLE_EDITABLE);
    m_tbZ.SetStyle(dTEXT_BOX_STYLE_EDITABLE);

    m_tbX.SetDefaultColor({ 210, 210, 210, 255 });
    m_tbY.SetDefaultColor({ 210, 210, 210, 255 });
    m_tbZ.SetDefaultColor({ 210, 210, 210, 255 });

    m_layout.LoadJson("Data/UI/inspector_panel.json");
    m_layout.Bind("tb_x", &m_tbX);
    m_layout.Bind("tb_y", &m_tbY);
    m_layout.Bind("tb_z", &m_tbZ);
}

// ─────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────

void IEInspectorPanel::SetFocus(IETextBox* tb)
{
    m_focusedBox = tb;
    IECore::SetFocusedTextBox(tb);
}

bool IEInspectorPanel::HitTest(const SDL_Rect& r, int32_t mx, int32_t my) const
{
    return mx >= r.x && mx < r.x + r.w && my >= r.y && my < r.y + r.h;
}

void IEInspectorPanel::SyncTextToTransform(IETextBox& tb, float value)
{
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%.2f", static_cast<double>(value));
    tb.SetText(buf);
}

// ─────────────────────────────────────────
// SetTarget
// ─────────────────────────────────────────

void IEInspectorPanel::SetTarget(IEGameObject* obj)
{
    if (m_target == obj)
        return;

    m_target = obj;

    if (m_target != nullptr)
    {
        auto* t = m_target->GetComponent<IETransformComponent>();
        if (t != nullptr)
        {
            SyncTextToTransform(m_tbX, t->GetX());
            SyncTextToTransform(m_tbY, t->GetY());
            SyncTextToTransform(m_tbZ, t->GetZ());
        }
    }
    else
    {
        m_tbX.SetText("");
        m_tbY.SetText("");
        m_tbZ.SetText("");
    }
}

// ─────────────────────────────────────────
// Propagation
// ─────────────────────────────────────────

void IEInspectorPanel::SetFont(IEFont* f)
{
    IEPanel::SetFont(f);
    m_layout.SetFont(f);
    m_tbX.SetFont(f);
    m_tbY.SetFont(f);
    m_tbZ.SetFont(f);
}

void IEInspectorPanel::SetOwnerWindow(IEWindow* w)
{
    IEPanel::SetOwnerWindow(w);
    m_layout.SetOwnerWindow(w);
    m_tbX.SetOwnerWindow(w);
    m_tbY.SetOwnerWindow(w);
    m_tbZ.SetOwnerWindow(w);
}

void IEInspectorPanel::SetRenderer(IERenderer* r)
{
    IEPanel::SetRenderer(r);
    m_layout.SetRenderer(r);
    m_tbX.SetRenderer(r);
    m_tbY.SetRenderer(r);
    m_tbZ.SetRenderer(r);
}

void IEInspectorPanel::SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    m_layout.SetContentRect(x, y, w, h);
}

// ─────────────────────────────────────────
// Update
// ─────────────────────────────────────────

void IEInspectorPanel::Update(float /*dt*/)
{
    if (m_target != nullptr)
    {
        auto* t = m_target->GetComponent<IETransformComponent>();
        if (t != nullptr)
        {
            auto applyOrSync = [&](IETextBox& tb, float cur, auto setter)
            {
                if (IECore::GetFocusedTextBox() == &tb)
                {
                    float val = 0.0f;
                    if (sscanf_s(tb.GetText(), "%f", &val) == 1)
                        setter(val);
                }
                else
                {
                    SyncTextToTransform(tb, cur);
                }
            };

            applyOrSync(m_tbX, t->GetX(), [t](float v){ t->SetX(v); });
            applyOrSync(m_tbY, t->GetY(), [t](float v){ t->SetY(v); });
            applyOrSync(m_tbZ, t->GetZ(), [t](float v){ t->SetZ(v); });
        }
    }

    // 클릭으로 텍스트박스 포커스 전환
    IEWindow* ownerWin = GetOwnerWindow();
    if (ownerWin != nullptr && IECore::GetMouseOnWindow() == ownerWin)
    {
        float gx = 0.0f, gy = 0.0f;
        SDL_MouseButtonFlags btn = SDL_GetGlobalMouseState(&gx, &gy);
        bool lmb = (btn & SDL_BUTTON_LMASK) != 0;

        if (lmb && !m_prevLMB)
        {
            int32_t winX = 0, winY = 0;
            SDL_GetWindowPosition(ownerWin->GetSDLWindow(), &winX, &winY);
            int32_t mx = static_cast<int32_t>(gx) - winX;
            int32_t my = static_cast<int32_t>(gy) - winY;

            SDL_Rect r = {};
            m_tbX.GetRect(r); if (HitTest(r, mx, my)) { SetFocus(&m_tbX); }
            m_tbY.GetRect(r); if (HitTest(r, mx, my)) { SetFocus(&m_tbY); }
            m_tbZ.GetRect(r); if (HitTest(r, mx, my)) { SetFocus(&m_tbZ); }
        }
        m_prevLMB = lmb;
    }

    m_layout.Update();
}

// ─────────────────────────────────────────
// Draw
// ─────────────────────────────────────────

static void DrawTbBg(IERenderer* r, IETextBox& tb, bool focused,
                     SDL_Color colBg, SDL_Color colBor, SDL_Color colFoc)
{
    SDL_Rect rect = {};
    tb.GetRect(rect);
    SDL_Color bor = focused ? colFoc : colBor;
    r->DrawRect(colBg, rect.x, rect.y, rect.w, rect.h, SDL_BLENDMODE_NONE);
    r->DrawLine(bor, rect.x,          rect.y,          rect.x + rect.w, rect.y);
    r->DrawLine(bor, rect.x,          rect.y + rect.h, rect.x + rect.w, rect.y + rect.h);
    r->DrawLine(bor, rect.x,          rect.y,          rect.x,          rect.y + rect.h);
    r->DrawLine(bor, rect.x + rect.w, rect.y,          rect.x + rect.w, rect.y + rect.h);
}

void IEInspectorPanel::Draw(IERenderer* r)
{
    if (r == nullptr)
        return;

    r->DrawRect(kColBg, m_layout.GetOriginX(), m_layout.GetOriginY(),
                m_layout.GetPanelW(), m_layout.GetPanelH(), SDL_BLENDMODE_NONE);

    if (m_target == nullptr)
    {
        IEFont* font = GetFont();
        if (font != nullptr)
            r->DrawText(font, IELocalize::Get("label.no_selection"), kColNoSel,
                        m_layout.GetOriginX() + 8, m_layout.GetOriginY() + 8);
        return;
    }

    DrawTbBg(r, m_tbX, m_focusedBox == &m_tbX, kColTbBg, kColTbBor, kColTbFoc);
    DrawTbBg(r, m_tbY, m_focusedBox == &m_tbY, kColTbBg, kColTbBor, kColTbFoc);
    DrawTbBg(r, m_tbZ, m_focusedBox == &m_tbZ, kColTbBg, kColTbBor, kColTbFoc);

    m_layout.Draw();
}
