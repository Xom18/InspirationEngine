#include "../InspirationEngine/InspirationEngine.h"
#include "IEInspectorPanel.h"

// ─────────────────────────────────────────
// SetTarget
// ─────────────────────────────────────────

void IEInspectorPanel::SetTarget(IEGameObject* obj)
{
    if (m_target == obj)
        return;

    m_target = obj;
    RebuildSections();
}

// ─────────────────────────────────────────
// Sections
// ─────────────────────────────────────────

void IEInspectorPanel::RebuildSections()
{
    m_tbName    = nullptr;
    m_slX       = nullptr;  m_slY    = nullptr;  m_slZ      = nullptr;
    m_slRot     = nullptr;  m_slSx   = nullptr;  m_slSy     = nullptr;
    m_tbAtlas   = nullptr;  m_tbTile = nullptr;
    m_ddCamType = nullptr;  m_slCamZoom = nullptr;

    m_sections.clear();

    if (m_target == nullptr)
        return;

    // Basic — 항상, non-collapsible
    {
        auto sec = std::make_unique<IESection>();
        sec->SetTitle("Basic");
        sec->SetCollapsible(false);
        m_tbName = sec->AddTextBox(IELocalize::Get("ui.name"));
        m_sections.push_back(std::move(sec));
    }

    // Transform
    auto* t = m_target->GetComponent<IETransformComponent>();
    if (t != nullptr)
    {
        auto sec = std::make_unique<IESection>();
        sec->SetTitle("Transform");
        sec->SetHeaderLabel("IETransformComponent");

        m_slX = sec->AddSlider(IELocalize::Get("ui.x"), -2000.0f, 2000.0f);
        m_slX->SetCallback([t](float v) { t->SetX(v); });

        m_slY = sec->AddSlider(IELocalize::Get("ui.y"), -2000.0f, 2000.0f);
        m_slY->SetCallback([t](float v) { t->SetY(v); });

        m_slZ = sec->AddSlider(IELocalize::Get("ui.z"), -500.0f, 500.0f);
        m_slZ->SetCallback([t](float v) { t->SetZ(v); });

        m_slRot = sec->AddSlider(IELocalize::Get("ui.rot"), -180.0f, 180.0f);
        m_slRot->SetCallback([t](float v) { t->SetRotation(v); });

        m_slSx = sec->AddSlider(IELocalize::Get("ui.scale_x"), 0.01f, 5.0f);
        m_slSx->SetCallback([t](float v) { t->SetScaleX(v); });

        m_slSy = sec->AddSlider(IELocalize::Get("ui.scale_y"), 0.01f, 5.0f);
        m_slSy->SetCallback([t](float v) { t->SetScaleY(v); });

        m_sections.push_back(std::move(sec));
    }

    // Tile
    auto* tile = m_target->GetComponent<IETileComponent>();
    if (tile != nullptr)
    {
        auto sec = std::make_unique<IESection>();
        sec->SetTitle("Tile");
        sec->SetHeaderLabel("IETileComponent");

        m_tbAtlas = sec->AddTextBox(IELocalize::Get("ui.atlas"));
        m_tbTile  = sec->AddTextBox(IELocalize::Get("ui.tile"));

        m_sections.push_back(std::move(sec));
    }

    // Camera
    auto* cc = m_target->GetComponent<IECameraComponent>();
    if (cc != nullptr)
    {
        auto sec = std::make_unique<IESection>();
        sec->SetTitle("Camera");
        sec->SetHeaderLabel("IECameraComponent");

        m_ddCamType = sec->AddDropdown("Type",
            { "TopView", "Isometric", "SideView", "OverheadOblique", "DepthSide" });
        m_ddCamType->SetCallback([cc](int32_t idx)
        {
            using CT = IECameraComponent::CameraType;
            switch (idx)
            {
            case 0: cc->SetType(CT::TopView);         break;
            case 1: cc->SetType(CT::Isometric);       break;
            case 2: cc->SetType(CT::SideView);        break;
            case 3: cc->SetType(CT::OverheadOblique); break;
            case 4: cc->SetType(CT::DepthSide);       break;
            default: break;
            }
        });

        m_slCamZoom = sec->AddSlider(IELocalize::Get("ui.zoom"), 0.05f, 20.0f);
        m_slCamZoom->SetCallback([cc](float v) { cc->SetZoom(v); });

        m_sections.push_back(std::move(sec));
    }

    PropagateContextToSections();
    m_scroll.ResetScroll();
    RelayoutSections();
    SyncFromTarget();
}

void IEInspectorPanel::RelayoutSections()
{
    int32_t cy     = m_y - m_scroll.GetScrollOffsetY();
    int32_t totalH = 0;

    for (auto& sec : m_sections)
    {
        int32_t sh = sec->Layout(m_x, cy, m_w - 8) + 2;
        cy     += sh;
        totalH += sh;
    }

    m_scroll.SetContentHeight(totalH);
}

void IEInspectorPanel::PropagateContextToSections()
{
    IEFont*     f = GetFont();
    IEWindow*   w = GetOwnerWindow();
    IERenderer* r = GetRenderer();

    for (auto& sec : m_sections)
    {
        sec->SetFont(f);
        sec->SetOwnerWindow(w);
        sec->SetRenderer(r);
    }
}

// ─────────────────────────────────────────
// Sync
// ─────────────────────────────────────────

void IEInspectorPanel::SyncFromTarget()
{
    if (m_target == nullptr)
        return;

    IETextBox* focused = IECore::GetFocusedTextBox();

    if (m_tbName != nullptr && focused != m_tbName)
        m_tbName->SetText(m_target->GetName().c_str());

    auto* t = m_target->GetComponent<IETransformComponent>();
    if (t != nullptr)
    {
        if (m_slX)   m_slX->SetValue(t->GetX());
        if (m_slY)   m_slY->SetValue(t->GetY());
        if (m_slZ)   m_slZ->SetValue(t->GetZ());
        if (m_slRot) m_slRot->SetValue(t->GetRotation());
        if (m_slSx)  m_slSx->SetValue(t->GetScaleX());
        if (m_slSy)  m_slSy->SetValue(t->GetScaleY());
    }

    auto* tile = m_target->GetComponent<IETileComponent>();
    if (tile != nullptr)
    {
        if (m_tbAtlas != nullptr && focused != m_tbAtlas)
            m_tbAtlas->SetText(tile->GetAtlas().c_str());
        if (m_tbTile != nullptr && focused != m_tbTile)
            m_tbTile->SetText(tile->GetTile().c_str());
    }

    auto* cc = m_target->GetComponent<IECameraComponent>();
    if (cc != nullptr)
    {
        if (m_slCamZoom != nullptr)
            m_slCamZoom->SetValue(cc->GetZoom());
        if (m_ddCamType != nullptr)
            m_ddCamType->SetSelectedIndex(static_cast<int32_t>(cc->GetType()));
    }
}

void IEInspectorPanel::ApplyFocusedInput()
{
    if (m_target == nullptr)
        return;

    IETextBox* focused = IECore::GetFocusedTextBox();

    if (m_tbName != nullptr && focused == m_tbName)
        m_target->SetName(m_tbName->GetText());

    auto* tile = m_target->GetComponent<IETileComponent>();

    if (m_tbAtlas != nullptr && focused == m_tbAtlas)
    {
        if (tile == nullptr)
            tile = m_target->AddComponent<IETileComponent>();
        tile->SetAtlas(m_tbAtlas->GetText());
    }

    if (m_tbTile != nullptr && focused == m_tbTile)
    {
        if (tile == nullptr)
            tile = m_target->GetComponent<IETileComponent>();
        if (tile != nullptr)
            tile->SetTile(m_tbTile->GetText());
    }
}

// ─────────────────────────────────────────
// Propagation
// ─────────────────────────────────────────

void IEInspectorPanel::SetFont(IEFont* f)
{
    IEPanel::SetFont(f);
    m_scroll.SetFont(f);
    PropagateContextToSections();
}

void IEInspectorPanel::SetOwnerWindow(IEWindow* w)
{
    IEPanel::SetOwnerWindow(w);
    m_scroll.SetOwnerWindow(w);
    PropagateContextToSections();
}

void IEInspectorPanel::SetRenderer(IERenderer* r)
{
    IEPanel::SetRenderer(r);
    m_scroll.SetRenderer(r);
    PropagateContextToSections();
}

void IEInspectorPanel::SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    m_x = x;  m_y = y;  m_w = w;  m_h = h;
    m_scroll.SetRect(x, y, w, h);
    RelayoutSections();
}

// ─────────────────────────────────────────
// Update
// ─────────────────────────────────────────

void IEInspectorPanel::Update(float /*dt*/)
{
    m_scroll.Update();
    RelayoutSections();
    ApplyFocusedInput();
    SyncFromTarget();

    for (auto& sec : m_sections)
        sec->Update();
}

// ─────────────────────────────────────────
// Draw
// ─────────────────────────────────────────

void IEInspectorPanel::Draw(IERenderer* r)
{
    if (r == nullptr)
        return;

    r->DrawRect(kColBg, m_x, m_y, m_w, m_h, SDL_BLENDMODE_NONE);

    if (m_target == nullptr)
    {
        IEFont* font = GetFont();
        if (font != nullptr)
            r->DrawText(font, IELocalize::Get("label.no_selection"),
                        kColNoSel, m_x + 8, m_y + 8);
        return;
    }

    m_scroll.BeginDraw();

    for (auto& sec : m_sections)
        sec->Draw();

    // 열린 Dropdown 팝업 — DrawOverlay 체인으로 최상위 재렌더
    for (auto& sec : m_sections)
        sec->DrawOverlay();

    m_scroll.EndDraw();

    m_scroll.Draw();
}
