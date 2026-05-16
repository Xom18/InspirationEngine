#include "IEMainEditorWindow.h"
#include "IEAtlasEditorWindow.h"
#include "IEFloatingPanelWindow.h"
#include <cstdio>
#include "../InspirationEngine/Scene/IESceneSerializer.h"

// ─────────────────────────────────────────
// 파일 내부 타입 alias
// ─────────────────────────────────────────

using SN    = IEMainEditorWindow::SplitNode;
using SNPtr = std::unique_ptr<SN>;

static SNPtr MakeLeaf(IEDockedPanel* dp)
{
    auto n = std::make_unique<SN>();
    n->isLeaf      = true;
    n->dockedPanel = dp;
    return n;
}

static SNPtr MakeBranch(SN::Dir dir, float ratio, SNPtr a, SNPtr b)
{
    auto n = std::make_unique<SN>();
    n->isLeaf = false;
    n->dir    = dir;
    n->ratio  = ratio;
    n->a      = std::move(a);
    n->b      = std::move(b);
    return n;
}

// ─────────────────────────────────────────
// Init
// ─────────────────────────────────────────

void IEMainEditorWindow::InitWindow(IEFont* font, IEAtlasEditorWindow* atlasEditor)
{
    m_font        = font;
    m_atlasEditor = atlasEditor;

    IERenderer* r = GetRenderer(0);

    m_btnAtlas.SetFont(font);
    m_btnAtlas.SetRenderer(r);
    m_btnAtlas.SetRect(8, 6, 110, 28);
    m_btnAtlas.SetLabel(IELocalize::Get("btn.atlas_editor"));
    m_btnAtlas.SetOwnerWindow(this);
    m_btnAtlas.SetCallback([this]() {
        if (m_atlasEditor != nullptr)
            m_atlasEditor->ShowWindow();
    });

    m_btnSave.SetFont(font);
    m_btnSave.SetRenderer(r);
    m_btnSave.SetRect(126, 6, 80, 28);
    m_btnSave.SetLabel(IELocalize::Get("btn.save"));
    m_btnSave.SetOwnerWindow(this);
    m_btnSave.SetCallback([this]() {
        if (m_vpPanel != nullptr)
            IESceneSerializer::Save(m_vpPanel->GetScene(), m_vpPanel->GetCamera(), kScenePath);
    });

    m_btnLoad.SetFont(font);
    m_btnLoad.SetRenderer(r);
    m_btnLoad.SetRect(214, 6, 80, 28);
    m_btnLoad.SetLabel(IELocalize::Get("btn.load"));
    m_btnLoad.SetOwnerWindow(this);
    m_btnLoad.SetCallback([this]() {
        if (m_vpPanel != nullptr)
        {
            IESceneSerializer::Load(m_vpPanel->GetScene(), m_vpPanel->GetCamera(), kScenePath);
            m_history.Clear();
            if (m_entityPanel != nullptr)
                m_entityPanel->RefreshList();
        }
    });

    m_btnAddStatic.SetFont(font);
    m_btnAddStatic.SetRenderer(r);
    m_btnAddStatic.SetRect(302, 6, 80, 28);
    m_btnAddStatic.SetLabel(IELocalize::Get("btn.add_static"));
    m_btnAddStatic.SetOwnerWindow(this);
    m_btnAddStatic.SetCallback([this]() {
        if (m_vpPanel != nullptr)
            m_vpPanel->AddObject("StaticObject");
    });

    m_btnAddEntity.SetFont(font);
    m_btnAddEntity.SetRenderer(r);
    m_btnAddEntity.SetRect(390, 6, 80, 28);
    m_btnAddEntity.SetLabel(IELocalize::Get("btn.add_entity"));
    m_btnAddEntity.SetOwnerWindow(this);
    m_btnAddEntity.SetCallback([this]() {
        if (m_vpPanel != nullptr)
            m_vpPanel->AddObject("Entity");
    });

    InitPanels(font, atlasEditor);

    if (m_vpPanel != nullptr)
    {
        m_vpPanel->SetCommandHistory(&m_history);

        auto* cam = m_vpPanel->GetCamera();
        if (cam != nullptr)
        {
            cam->SetPosition(IEProjectConfig::GetDefaultCamX(), IEProjectConfig::GetDefaultCamY());
            cam->SetZoom(IEProjectConfig::GetDefaultCamZoom());
        }
        m_vpPanel->SetGridVisible(IEProjectConfig::IsGridVisible());
    }

    BuildLayoutTree();
    LayoutPanels();
}

void IEMainEditorWindow::InitPanels(IEFont* font, IEAtlasEditorWindow* atlasEditor)
{
    IERenderer* r = GetRenderer(0);

    auto setup = [&](IEDockedPanel& dp) {
        dp.SetFont(font);
        dp.SetOwnerWindow(this);
        dp.SetRenderer(r);
    };

    // ── Viewport ──────────────────────────────
    {
        auto vpPtr = std::make_unique<IEViewportPanel>();
        m_vpPanel  = vpPtr.get();
        m_panels.emplace_back(std::move(vpPtr));
        setup(m_panels.back());
    }

    // ── FileBrowser ───────────────────────────
    {
        auto fbPtr = std::make_unique<IEFileBrowserPanel>();
        fbPtr->SetAtlasEditor(atlasEditor);
        m_panels.emplace_back(std::move(fbPtr));
        setup(m_panels.back());
    }

    // ── Camera ────────────────────────────────
    {
        auto camPtr = std::make_unique<IECameraPanel>();
        m_camPanel  = camPtr.get();
        camPtr->SetCamera(m_vpPanel->GetCamera());
        m_panels.emplace_back(std::move(camPtr));
        setup(m_panels.back());
    }

    // ── Hierarchy ─────────────────────────────
    {
        auto elPtr    = std::make_unique<IEHierarchy>();
        m_entityPanel = elPtr.get();
        elPtr->SetScene(&m_vpPanel->GetScene());
        m_panels.emplace_back(std::move(elPtr));
        setup(m_panels.back());
    }

    // ── Inspector ─────────────────────────────
    {
        auto inspPtr = std::make_unique<IEInspectorPanel>();
        m_inspPanel  = inspPtr.get();
        m_panels.emplace_back(std::move(inspPtr));
        setup(m_panels.back());
    }
}

void IEMainEditorWindow::BuildLayoutTree()
{
    // m_panels 삽입 순서: Viewport, FileBrowser, Camera, Hierarchy, Inspector
    auto it = m_panels.begin();
    IEDockedPanel* vp   = &*it++;
    IEDockedPanel* fb   = &*it++;
    IEDockedPanel* cam  = &*it++;
    IEDockedPanel* hier = &*it++;
    IEDockedPanel* insp = &*it;

    constexpr float kLeftW  = 220.0f;
    constexpr float kRightW = 260.0f;
    constexpr float kWinW   = 1280.0f;

    float lRatio = kLeftW / kWinW;
    float rRatio = (kWinW - kLeftW - kRightW) / (kWinW - kLeftW);

    auto leftSplit  = MakeBranch(SN::Dir::V, 0.5f, MakeLeaf(fb),  MakeLeaf(cam));
    auto rightSplit = MakeBranch(SN::Dir::V, 0.5f, MakeLeaf(hier), MakeLeaf(insp));
    auto rightPart  = MakeBranch(SN::Dir::H, rRatio, MakeLeaf(vp), std::move(rightSplit));

    m_layoutRoot = MakeBranch(SN::Dir::H, lRatio, std::move(leftSplit), std::move(rightPart));
}

// ─────────────────────────────────────────
// Layout
// ─────────────────────────────────────────

void IEMainEditorWindow::LayoutPanels()
{
    if (m_layoutRoot == nullptr)
        return;
    SDL_Rect body{ 0, kMenuH, GetWidth(), GetHeight() - kMenuH };
    LayoutTree(m_layoutRoot.get(), body);
}

void IEMainEditorWindow::LayoutTree(SplitNode* node, SDL_Rect rect)
{
    node->rect = rect;
    if (node->isLeaf)
    {
        if (node->dockedPanel != nullptr)
            node->dockedPanel->SetRect(rect.x, rect.y, rect.w, rect.h);
        return;
    }

    SDL_Rect ra = rect, rb = rect;
    if (node->dir == SplitNode::Dir::H)
    {
        int32_t avail  = rect.w - kSepW;
        int32_t splitA = std::max(kMinColW, std::min(static_cast<int32_t>(avail * node->ratio), avail - kMinColW));
        ra.w = splitA;
        rb.x = rect.x + splitA + kSepW;
        rb.w = rect.w - splitA - kSepW;
        node->sep = { rect.x + splitA, rect.y, kSepW, rect.h };
    }
    else
    {
        int32_t avail  = rect.h - kSepW;
        int32_t splitA = std::max(kMinColW, std::min(static_cast<int32_t>(avail * node->ratio), avail - kMinColW));
        ra.h = splitA;
        rb.y = rect.y + splitA + kSepW;
        rb.h = rect.h - splitA - kSepW;
        node->sep = { rect.x, rect.y + splitA, rect.w, kSepW };
    }

    LayoutTree(node->a.get(), ra);
    LayoutTree(node->b.get(), rb);
}

// ─────────────────────────────────────────
// Resize / X button
// ─────────────────────────────────────────

void IEMainEditorWindow::OnResize(int32_t /*w*/, int32_t /*h*/)
{
    LayoutPanels();
}

void IEMainEditorWindow::CallXButton()
{
    IECore::StopEngine();
}

// ─────────────────────────────────────────
// Draw helpers
// ─────────────────────────────────────────

void IEMainEditorWindow::DrawSeps(SplitNode* node, IERenderer* r)
{
    if (node == nullptr || node->isLeaf)
        return;
    SDL_Color c = (m_sepDragNode == node) ? kColSepHot : kColSep;
    r->DrawRect(c, node->sep.x, node->sep.y, node->sep.w, node->sep.h, SDL_BLENDMODE_NONE);
    DrawSeps(node->a.get(), r);
    DrawSeps(node->b.get(), r);
}

void IEMainEditorWindow::DrawDropZones(SplitNode* node, IERenderer* r,
                                        IEDockedPanel* target, EDropSide side)
{
    if (node == nullptr)
        return;
    if (node->isLeaf && node->dockedPanel == target)
    {
        SDL_Rect zone = ComputeZoneRect(node->rect, side);
        r->DrawRect(kColDropHot, zone.x, zone.y, zone.w, zone.h, SDL_BLENDMODE_BLEND);
        return;
    }
    DrawDropZones(node->a.get(), r, target, side);
    DrawDropZones(node->b.get(), r, target, side);
}

IEMainEditorWindow::EDropSide IEMainEditorWindow::ComputeDropSide(SDL_Rect rect, int32_t mx, int32_t my)
{
    // 가장자리 25% 슬랩 → 방향, 나머지 중앙 → Center
    int32_t dx = mx - rect.x;
    int32_t dy = my - rect.y;
    int32_t slabX = rect.w / 4;
    int32_t slabY = rect.h / 4;

    if (dx < slabX)                 return EDropSide::Left;
    if (dx >= rect.w - slabX)       return EDropSide::Right;
    if (dy < slabY)                 return EDropSide::Top;
    if (dy >= rect.h - slabY)       return EDropSide::Bottom;
    return EDropSide::Center;
}

SDL_Rect IEMainEditorWindow::ComputeZoneRect(SDL_Rect rect, EDropSide side)
{
    int32_t hw = rect.w / 2;
    int32_t hh = rect.h / 2;
    switch (side)
    {
        case EDropSide::Left:   return { rect.x,          rect.y,          hw, rect.h };
        case EDropSide::Right:  return { rect.x + hw,     rect.y,          hw, rect.h };
        case EDropSide::Top:    return { rect.x,           rect.y,         rect.w, hh };
        case EDropSide::Bottom: return { rect.x,           rect.y + hh,    rect.w, hh };
        default:                return rect;  // Center
    }
}

// ─────────────────────────────────────────
// Find helpers
// ─────────────────────────────────────────

IEMainEditorWindow::SplitNode* IEMainEditorWindow::FindSepAt(SplitNode* node, int32_t mx, int32_t my)
{
    if (node == nullptr || node->isLeaf)
        return nullptr;

    SDL_Rect hit = node->sep;
    hit.x -= kSepHitW; hit.w += kSepHitW * 2;
    hit.y -= kSepHitW; hit.h += kSepHitW * 2;
    if (mx >= hit.x && mx < hit.x + hit.w && my >= hit.y && my < hit.y + hit.h)
        return node;

    if (auto* found = FindSepAt(node->a.get(), mx, my))
        return found;
    return FindSepAt(node->b.get(), mx, my);
}

IEMainEditorWindow::SplitNode* IEMainEditorWindow::FindLeafAt(SplitNode* node, int32_t mx, int32_t my)
{
    if (node == nullptr)
        return nullptr;
    SDL_Rect& rc = node->rect;
    if (mx < rc.x || mx >= rc.x + rc.w || my < rc.y || my >= rc.y + rc.h)
        return nullptr;
    if (node->isLeaf)
        return node;
    if (auto* n = FindLeafAt(node->a.get(), mx, my))
        return n;
    return FindLeafAt(node->b.get(), mx, my);
}

IEMainEditorWindow::SplitNode* IEMainEditorWindow::FindAnyLeaf(SplitNode* node)
{
    if (node == nullptr)
        return nullptr;
    if (node->isLeaf)
        return node;
    if (auto* n = FindAnyLeaf(node->b.get()))
        return n;
    return FindAnyLeaf(node->a.get());
}

IEMainEditorWindow::SplitNode* IEMainEditorWindow::FindLeafNode(SplitNode* node, IEDockedPanel* panel)
{
    if (node == nullptr)
        return nullptr;
    if (node->isLeaf)
        return (node->dockedPanel == panel) ? node : nullptr;
    if (auto* n = FindLeafNode(node->a.get(), panel))
        return n;
    return FindLeafNode(node->b.get(), panel);
}

// ─────────────────────────────────────────
// Tree mutation
// ─────────────────────────────────────────

void IEMainEditorWindow::RemoveLeaf(IEDockedPanel* panel)
{
    if (m_layoutRoot == nullptr)
        return;

    // 루트 자체가 leaf 인 경우
    if (m_layoutRoot->isLeaf && m_layoutRoot->dockedPanel == panel)
    {
        m_layoutRoot.reset();
        for (auto it = m_panels.begin(); it != m_panels.end(); ++it)
        {
            if (&*it == panel) { m_panels.erase(it); break; }
        }
        return;
    }

    // 부모를 찾아 형제로 대체하는 재귀 람다
    std::function<bool(std::unique_ptr<SplitNode>&)> collapse =
        [&](std::unique_ptr<SplitNode>& nodeRef) -> bool
    {
        SplitNode* node = nodeRef.get();
        if (node == nullptr || node->isLeaf)
            return false;

        auto check = [&](std::unique_ptr<SplitNode>& child,
                         std::unique_ptr<SplitNode>& sibling) -> bool
        {
            if (child->isLeaf && child->dockedPanel == panel)
            {
                // child 제거 → sibling 이 parent 위치를 차지
                nodeRef = std::move(sibling);
                return true;
            }
            return false;
        };

        if (check(node->a, node->b)) return true;
        if (check(node->b, node->a)) return true;
        if (collapse(node->a))       return true;
        return collapse(node->b);
    };

    collapse(m_layoutRoot);

    for (auto it = m_panels.begin(); it != m_panels.end(); ++it)
    {
        if (&*it == panel) { m_panels.erase(it); break; }
    }

    RescanRawPtrs();
    LayoutPanels();
}

void IEMainEditorWindow::InsertLeaf(IEDockedPanel* newPanel, IEDockedPanel* target, EDropSide side)
{
    if (target == nullptr || m_layoutRoot == nullptr)
        return;

    // target leaf 찾기
    std::function<bool(std::unique_ptr<SplitNode>&)> insert =
        [&](std::unique_ptr<SplitNode>& nodeRef) -> bool
    {
        SplitNode* node = nodeRef.get();
        if (node == nullptr)
            return false;

        if (node->isLeaf && node->dockedPanel == target)
        {
            if (side == EDropSide::Center)
            {
                // 기존 패널과 교체 (새 패널이 이 위치 차지)
                node->dockedPanel = newPanel;
                // 기존 패널은 이미 m_panels 에 있고 tree 에서 빠지는 것
                // → 기존 패널이 orphan 이 되어 레이아웃에서 사라짐
                // 기존 패널 제거를 별도로 해야 함 (여기선 ptr 교체만)
                return true;
            }

            SN::Dir dir = (side == EDropSide::Left || side == EDropSide::Right) ? SN::Dir::H : SN::Dir::V;
            bool newFirst = (side == EDropSide::Left || side == EDropSide::Top);

            auto oldLeaf = MakeLeaf(target);
            auto newLeaf = MakeLeaf(newPanel);
            nodeRef = MakeBranch(dir, 0.5f,
                newFirst ? std::move(newLeaf) : std::move(oldLeaf),
                newFirst ? std::move(oldLeaf) : std::move(newLeaf));
            return true;
        }

        if (insert(node->a)) return true;
        return insert(node->b);
    };

    insert(m_layoutRoot);
    LayoutPanels();
}

// ─────────────────────────────────────────
// RescanRawPtrs
// ─────────────────────────────────────────

void IEMainEditorWindow::RescanRawPtrs()
{
    m_vpPanel     = nullptr;
    m_camPanel    = nullptr;
    m_entityPanel = nullptr;
    m_inspPanel   = nullptr;
    for (auto& dp : m_panels)
    {
        IEPanel* p = dp.GetPanel();
        if (p == nullptr) continue;
        if (auto* vp   = dynamic_cast<IEViewportPanel*>(p))   m_vpPanel     = vp;
        if (auto* cam  = dynamic_cast<IECameraPanel*>(p))     m_camPanel    = cam;
        if (auto* hier = dynamic_cast<IEHierarchy*>(p))       m_entityPanel = hier;
        if (auto* insp = dynamic_cast<IEInspectorPanel*>(p))  m_inspPanel   = insp;
    }
}

// ─────────────────────────────────────────
// ProcessUndock
// ─────────────────────────────────────────

void IEMainEditorWindow::ProcessUndock()
{
    for (auto& dp : m_panels)
    {
        if (!dp.ShouldUndock())
            continue;

        dp.ClearUndockFlag();

        int32_t fx = dp.GetUndockScreenX();
        int32_t fy = dp.GetUndockScreenY();
        int32_t fw = dp.GetUndockW();
        int32_t fh = dp.GetUndockH();

        IEDockedPanel* dpPtr = &dp;
        auto panelPtr = dp.ReleasePanel();

        RemoveLeaf(dpPtr);  // m_panels 에서도 제거됨

        std::string winId = "float_" + std::to_string(m_floatIdCounter.fetch_add(1));

        struct CreateTask
        {
            std::unique_ptr<IEPanel> panel;
            std::string              title;
            int32_t                  fw, fh, fx, fy;
            IEFont*                  font;
            std::string              windowId;
        };
        auto task      = std::make_shared<CreateTask>();
        task->title    = panelPtr->GetTitle();
        task->panel    = std::move(panelPtr);
        task->fw       = fw;
        task->fh       = fh;
        task->fx       = fx;
        task->fy       = fy;
        task->font     = m_font;
        task->windowId = winId;

        IECore::PostMainThreadTask([task, this]() {
            auto* floatWin = new IEFloatingPanelWindow();
            floatWin->CreateWindow(task->title.c_str(), task->fw, task->fh, task->fx, task->fy,
                static_cast<SDL_WindowFlags>(SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS));
            floatWin->Init(std::move(task->panel), task->font, task->windowId);

            floatWin->SetDropDockCallback([this](std::unique_ptr<IEPanel> p,
                                                  IEDockedPanel* target, int32_t side) {
                Redock(std::move(p), target, side);
            });
            floatWin->SetRedockCallback([this](std::unique_ptr<IEPanel> p) {
                Redock(std::move(p), nullptr, static_cast<int32_t>(EDropSide::Right));
            });

            IECore::RequestAddWindow(task->windowId.c_str(), floatWin);
        });

        break; // 한 프레임에 하나만 처리
    }
}

// ─────────────────────────────────────────
// Redock
// ─────────────────────────────────────────

void IEMainEditorWindow::Redock(std::unique_ptr<IEPanel> panel,
                                  IEDockedPanel* target, int32_t side)
{
    IERenderer* r = GetRenderer(0);

    m_panels.emplace_back(std::move(panel));
    IEDockedPanel* newDocked = &m_panels.back();
    newDocked->SetFont(m_font);
    newDocked->SetOwnerWindow(this);
    newDocked->SetRenderer(r);

    RescanRawPtrs();

    // 트리가 비어있으면 루트 leaf 로
    if (m_layoutRoot == nullptr)
    {
        m_layoutRoot = MakeLeaf(newDocked);
        LayoutPanels();
        return;
    }

    IEDockedPanel* insertTarget = target;
    if (insertTarget == nullptr)
    {
        SplitNode* leaf = FindAnyLeaf(m_layoutRoot.get());
        insertTarget = (leaf != nullptr) ? leaf->dockedPanel : nullptr;
    }

    if (insertTarget == nullptr)
    {
        m_layoutRoot = MakeLeaf(newDocked);
        LayoutPanels();
        return;
    }

    InsertLeaf(newDocked, insertTarget, static_cast<EDropSide>(side));
}

// ─────────────────────────────────────────
// Update
// ─────────────────────────────────────────

void IEMainEditorWindow::Update(float deltaTime)
{
    m_btnAtlas.Update();
    m_btnSave.Update();
    m_btnLoad.Update();
    m_btnAddStatic.Update();
    m_btnAddEntity.Update();

    IEInput& input = IECore::GetInput();
    bool ctrlHeld = input.GetKeyState(SDL_SCANCODE_LCTRL) || input.GetKeyState(SDL_SCANCODE_RCTRL);
    bool sDown    = input.GetKeyState(SDL_SCANCODE_S);
    bool oDown    = input.GetKeyState(SDL_SCANCODE_O);

    if (ctrlHeld && sDown && !m_prevCtrlS)
    {
        if (m_vpPanel != nullptr)
            IESceneSerializer::Save(m_vpPanel->GetScene(), m_vpPanel->GetCamera(), kScenePath);
    }
    if (ctrlHeld && oDown && !m_prevCtrlO)
    {
        if (m_vpPanel != nullptr)
        {
            IESceneSerializer::Load(m_vpPanel->GetScene(), m_vpPanel->GetCamera(), kScenePath);
            if (m_entityPanel != nullptr)
                m_entityPanel->RefreshList();
        }
    }

    m_prevCtrlS = ctrlHeld && sDown;
    m_prevCtrlO = ctrlHeld && oDown;

    bool zDown = input.GetKeyState(SDL_SCANCODE_Z);
    if (ctrlHeld && zDown && !m_prevCtrlZ)
        m_history.Undo();
    m_prevCtrlZ = ctrlHeld && zDown;

    bool yDown = input.GetKeyState(SDL_SCANCODE_Y);
    if (ctrlHeld && yDown && !m_prevCtrlY)
        m_history.Redo();
    m_prevCtrlY = ctrlHeld && yDown;

    bool delDown = input.GetKeyState(SDL_SCANCODE_DELETE);
    if (delDown && !m_prevDelete)
    {
        if (m_vpPanel != nullptr)
            m_vpPanel->DeleteSelectedObject();
    }
    m_prevDelete = delDown;

    // Hierarchy → Viewport 선택 동기화
    if (m_entityPanel != nullptr && m_vpPanel != nullptr)
    {
        int32_t hierIdx = m_entityPanel->GetSelectedIndex();
        if (hierIdx != m_prevHierIdx)
        {
            if (hierIdx >= 0)
            {
                const auto& objs = m_vpPanel->GetScene().GetObjects();
                if (hierIdx < static_cast<int32_t>(objs.size()))
                    m_vpPanel->SetSelectedObject(objs[hierIdx].get());
                else
                    m_vpPanel->SetSelectedObject(nullptr);
            }
            else
            {
                m_vpPanel->SetSelectedObject(nullptr);
            }
            m_prevHierIdx = hierIdx;
        }
    }

    // Viewport → Hierarchy 역방향 동기화
    if (m_entityPanel != nullptr && m_vpPanel != nullptr)
    {
        IEGameObject* vpSel = m_vpPanel->GetSelectedObject();
        if (vpSel != m_prevVpSelected)
        {
            m_prevVpSelected = vpSel;
            if (vpSel != nullptr)
            {
                const auto& objs = m_vpPanel->GetScene().GetObjects();
                int32_t idx = -1;
                for (int32_t i = 0; i < static_cast<int32_t>(objs.size()); ++i)
                {
                    if (objs[i].get() == vpSel) { idx = i; break; }
                }
                m_entityPanel->SetSelectedIndex(idx);
                m_prevHierIdx = idx;
            }
            else
            {
                m_entityPanel->SetSelectedIndex(-1);
                m_prevHierIdx = -1;
            }
        }
    }

    if (m_inspPanel != nullptr && m_vpPanel != nullptr)
        m_inspPanel->SetTarget(m_vpPanel->GetSelectedObject());

    // 패널 업데이트
    for (auto& dp : m_panels)
    {
        if (dp.IsAlive())
            dp.Update(deltaTime);
    }

    ProcessUndock();

    // ── 구분선 드래그 ─────────────────────────────────────────────
    {
        float gxF = 0.0f, gyF = 0.0f;
        SDL_MouseButtonFlags btnState = SDL_GetGlobalMouseState(&gxF, &gyF);
        int32_t winX = 0, winY = 0;
        SDL_GetWindowPosition(GetSDLWindow(), &winX, &winY);
        int32_t mx = static_cast<int32_t>(gxF) - winX;
        int32_t my = static_cast<int32_t>(gxF - winX);  // 초기화용 (실제값은 아래)
        int32_t gx = static_cast<int32_t>(gxF);
        int32_t gy = static_cast<int32_t>(gyF);
        mx = gx - winX;
        my = gy - winY;

        bool lmb      = (btnState & SDL_BUTTON_LMASK) != 0;
        bool lmbClick = lmb && !m_prevGlobalLMB;
        m_prevGlobalLMB = lmb;

        if (!lmb)
        {
            m_sepDragNode = nullptr;
        }
        else if (m_sepDragNode == nullptr && lmbClick && my > kMenuH)
        {
            m_sepDragNode = FindSepAt(m_layoutRoot.get(), mx, my);
            if (m_sepDragNode != nullptr)
            {
                m_sepDragStartRatio = m_sepDragNode->ratio;
                m_sepDragStartCoord = (m_sepDragNode->dir == SplitNode::Dir::H) ? gx : gy;
            }
        }

        if (m_sepDragNode != nullptr)
        {
            SDL_Rect nr = m_sepDragNode->rect;
            if (m_sepDragNode->dir == SplitNode::Dir::H)
            {
                float delta = static_cast<float>(gx - m_sepDragStartCoord) / nr.w;
                m_sepDragNode->ratio = std::max(0.05f, std::min(m_sepDragStartRatio + delta, 0.95f));
            }
            else
            {
                float delta = static_cast<float>(gy - m_sepDragStartCoord) / nr.h;
                m_sepDragNode->ratio = std::max(0.05f, std::min(m_sepDragStartRatio + delta, 0.95f));
            }
            LayoutPanels();
        }
    }

    // ── 플로팅 드래그 중 드롭 타겟 갱신 ─────────────────────────────
    if (IECore::IsFloatDragging())
    {
        int32_t winX = 0, winY = 0;
        SDL_GetWindowPosition(GetSDLWindow(), &winX, &winY);
        int32_t mx = IECore::GetFloatDragGX() - winX;
        int32_t my = IECore::GetFloatDragGY() - winY;

        SplitNode* leaf = FindLeafAt(m_layoutRoot.get(), mx, my);
        if (leaf != nullptr && leaf->dockedPanel != nullptr)
        {
            EDropSide side = ComputeDropSide(leaf->rect, mx, my);
            IECore::SetDropTarget(leaf->dockedPanel, static_cast<int32_t>(side));
        }
        else
        {
            IECore::ClearDropTarget();
        }
    }
    else
    {
        IECore::ClearDropTarget();
    }
}

// ─────────────────────────────────────────
// Draw
// ─────────────────────────────────────────

void IEMainEditorWindow::Draw()
{
    IERenderer* r = GetRenderer(0);
    if (r == nullptr)
        return;

    const int32_t winW = GetWidth();
    const int32_t winH = GetHeight();

    r->DrawRect(kColBg,   0, 0,      winW, winH,   SDL_BLENDMODE_NONE);
    r->DrawRect(kColMenu, 0, 0,      winW, kMenuH, SDL_BLENDMODE_NONE);
    r->DrawLine(kColSep,  0, kMenuH, winW, kMenuH);

    m_btnAtlas.Draw();
    m_btnSave.Draw();
    m_btnLoad.Draw();
    m_btnAddStatic.Draw();
    m_btnAddEntity.Draw();

    for (auto& dp : m_panels)
    {
        if (dp.IsAlive())
            dp.Draw();
    }

    DrawSeps(m_layoutRoot.get(), r);

    if (IECore::IsFloatDragging())
    {
        IEDockedPanel* target = IECore::GetDropTargetPanel();
        if (target != nullptr)
        {
            EDropSide side = static_cast<EDropSide>(IECore::GetDropTargetSide());
            DrawDropZones(m_layoutRoot.get(), r, target, side);
        }
    }
}
