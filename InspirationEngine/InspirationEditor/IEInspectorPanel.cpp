#include "../InspirationEngine/InspirationEngine.h"
#include "IEInspectorPanel.h"
#include <cstdio>

// ─────────────────────────────────────────
// Init
// ─────────────────────────────────────────

IEInspectorPanel::IEInspectorPanel()
{
    constexpr SDL_Color kTextCol = { 210, 210, 210, 255 };

    auto initTb = [&](IETextBox& tb)
    {
        tb.SetStyle(dTEXT_BOX_STYLE_EDITABLE);
        tb.SetDefaultColor(kTextCol);
    };

    initTb(m_tbName);
    initTb(m_tbX);     initTb(m_tbY);     initTb(m_tbZ);
    initTb(m_tbRot);   initTb(m_tbScaleX); initTb(m_tbScaleY);
    initTb(m_tbAtlas); initTb(m_tbTile);

    m_layout.LoadJson("Data/UI/inspector_panel.json");
    m_layout.Bind("tb_name",   &m_tbName);
    m_layout.Bind("tb_x",      &m_tbX);
    m_layout.Bind("tb_y",      &m_tbY);
    m_layout.Bind("tb_z",      &m_tbZ);
    m_layout.Bind("tb_rot",    &m_tbRot);
    m_layout.Bind("tb_scaleX", &m_tbScaleX);
    m_layout.Bind("tb_scaleY", &m_tbScaleY);
    m_layout.Bind("tb_atlas",  &m_tbAtlas);
    m_layout.Bind("tb_tile",   &m_tbTile);
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

void IEInspectorPanel::SyncFloat(IETextBox& tb, float value)
{
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%.2f", static_cast<double>(value));
    tb.SetText(buf);
}

void IEInspectorPanel::SyncStr(IETextBox& tb, const std::string& value)
{
    tb.SetText(value.c_str());
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
        SyncStr(m_tbName, m_target->GetName());

        auto* t = m_target->GetComponent<IETransformComponent>();
        if (t != nullptr)
        {
            SyncFloat(m_tbX,      t->GetX());
            SyncFloat(m_tbY,      t->GetY());
            SyncFloat(m_tbZ,      t->GetZ());
            SyncFloat(m_tbRot,    t->GetRotation());
            SyncFloat(m_tbScaleX, t->GetScaleX());
            SyncFloat(m_tbScaleY, t->GetScaleY());
        }

        auto* tile = m_target->GetComponent<IETileComponent>();
        if (tile != nullptr)
        {
            SyncStr(m_tbAtlas, tile->GetAtlas());
            SyncStr(m_tbTile,  tile->GetTile());
        }
        else
        {
            m_tbAtlas.SetText("");
            m_tbTile.SetText("");
        }
    }
    else
    {
        m_tbName.SetText("");
        m_tbX.SetText("");    m_tbY.SetText("");    m_tbZ.SetText("");
        m_tbRot.SetText("");  m_tbScaleX.SetText(""); m_tbScaleY.SetText("");
        m_tbAtlas.SetText(""); m_tbTile.SetText("");
    }
}

// ─────────────────────────────────────────
// Propagation
// ─────────────────────────────────────────

void IEInspectorPanel::SetFont(IEFont* f)
{
    IEPanel::SetFont(f);
    m_layout.SetFont(f);
    m_tbName.SetFont(f);
    m_tbX.SetFont(f);     m_tbY.SetFont(f);     m_tbZ.SetFont(f);
    m_tbRot.SetFont(f);   m_tbScaleX.SetFont(f); m_tbScaleY.SetFont(f);
    m_tbAtlas.SetFont(f); m_tbTile.SetFont(f);
}

void IEInspectorPanel::SetOwnerWindow(IEWindow* w)
{
    IEPanel::SetOwnerWindow(w);
    m_layout.SetOwnerWindow(w);
    m_tbName.SetOwnerWindow(w);
    m_tbX.SetOwnerWindow(w);     m_tbY.SetOwnerWindow(w);     m_tbZ.SetOwnerWindow(w);
    m_tbRot.SetOwnerWindow(w);   m_tbScaleX.SetOwnerWindow(w); m_tbScaleY.SetOwnerWindow(w);
    m_tbAtlas.SetOwnerWindow(w); m_tbTile.SetOwnerWindow(w);
}

void IEInspectorPanel::SetRenderer(IERenderer* r)
{
    IEPanel::SetRenderer(r);
    m_layout.SetRenderer(r);
    m_tbName.SetRenderer(r);
    m_tbX.SetRenderer(r);     m_tbY.SetRenderer(r);     m_tbZ.SetRenderer(r);
    m_tbRot.SetRenderer(r);   m_tbScaleX.SetRenderer(r); m_tbScaleY.SetRenderer(r);
    m_tbAtlas.SetRenderer(r); m_tbTile.SetRenderer(r);
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
        // Name
        if (IECore::GetFocusedTextBox() == &m_tbName)
            m_target->SetName(m_tbName.GetText());
        else
            SyncStr(m_tbName, m_target->GetName());

        // Transform
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
                    SyncFloat(tb, cur);
                }
            };

            applyOrSync(m_tbX,      t->GetX(),        [t](float v){ t->SetX(v); });
            applyOrSync(m_tbY,      t->GetY(),        [t](float v){ t->SetY(v); });
            applyOrSync(m_tbZ,      t->GetZ(),        [t](float v){ t->SetZ(v); });
            applyOrSync(m_tbRot,    t->GetRotation(), [t](float v){ t->SetRotation(v); });
            applyOrSync(m_tbScaleX, t->GetScaleX(),   [t](float v){ t->SetScaleX(v); });
            applyOrSync(m_tbScaleY, t->GetScaleY(),   [t](float v){ t->SetScaleY(v); });
        }

        // Tile
        auto* tile         = m_target->GetComponent<IETileComponent>();
        bool atlasFocused  = IECore::GetFocusedTextBox() == &m_tbAtlas;
        bool tileFocused   = IECore::GetFocusedTextBox() == &m_tbTile;

        if (atlasFocused || tileFocused)
        {
            if (tile == nullptr)
                tile = m_target->AddComponent<IETileComponent>();
            if (atlasFocused)
                tile->SetAtlas(m_tbAtlas.GetText());
            if (tileFocused)
                tile->SetTile(m_tbTile.GetText());
        }
        else if (tile != nullptr)
        {
            SyncStr(m_tbAtlas, tile->GetAtlas());
            SyncStr(m_tbTile,  tile->GetTile());
        }
        else
        {
            if (m_tbAtlas.GetText()[0] != '\0') m_tbAtlas.SetText("");
            if (m_tbTile.GetText()[0]  != '\0') m_tbTile.SetText("");
        }
    }

    // 클릭으로 포커스 전환
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
            auto checkFocus = [&](IETextBox& tb)
            {
                tb.GetRect(r);
                if (HitTest(r, mx, my))
                    SetFocus(&tb);
            };
            checkFocus(m_tbName);
            checkFocus(m_tbX);     checkFocus(m_tbY);     checkFocus(m_tbZ);
            checkFocus(m_tbRot);   checkFocus(m_tbScaleX); checkFocus(m_tbScaleY);
            checkFocus(m_tbAtlas); checkFocus(m_tbTile);
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

    auto drawTb = [&](IETextBox& tb)
    {
        DrawTbBg(r, tb, m_focusedBox == &tb, kColTbBg, kColTbBor, kColTbFoc);
    };

    drawTb(m_tbName);
    drawTb(m_tbX);     drawTb(m_tbY);     drawTb(m_tbZ);
    drawTb(m_tbRot);   drawTb(m_tbScaleX); drawTb(m_tbScaleY);
    drawTb(m_tbAtlas); drawTb(m_tbTile);

    m_layout.Draw();
}
