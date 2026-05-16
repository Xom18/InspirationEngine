#include "IEMainEditorWindow.h"
#include "IEAtlasEditorWindow.h"
#include "IEFloatingPanelWindow.h"
#include <cstdio>
#include "../InspirationEngine/Scene/IESceneSerializer.h"

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

        // 프로젝트 설정 기본값 적용 (scene.json 로드 전 초기 상태)
        auto* cam = m_vpPanel->GetCamera();
        if (cam != nullptr)
        {
            cam->SetPosition(IEProjectConfig::GetDefaultCamX(), IEProjectConfig::GetDefaultCamY());
            cam->SetZoom(IEProjectConfig::GetDefaultCamZoom());
        }
        m_vpPanel->SetGridVisible(IEProjectConfig::IsGridVisible());
    }

    LayoutPanels();
}

void IEMainEditorWindow::InitPanels(IEFont* font, IEAtlasEditorWindow* atlasEditor)
{
    IERenderer* r = GetRenderer(0);

    // ── Viewport ──────────────────────────────
    auto vpPtr  = std::make_unique<IEViewportPanel>();
    m_vpPanel   = vpPtr.get();

    auto vpDoc = IEDockedPanel(std::move(vpPtr));
    vpDoc.SetFont(font);
    vpDoc.SetOwnerWindow(this);
    vpDoc.SetRenderer(r);
    m_panels.push_back(std::move(vpDoc));
    m_slots.push_back({ EPanelColumn::Center, 0 });

    // ── FileBrowser ───────────────────────────
    auto fbPtr = std::make_unique<IEFileBrowserPanel>();
    fbPtr->SetAtlasEditor(atlasEditor);

    auto fbDoc = IEDockedPanel(std::move(fbPtr));
    fbDoc.SetFont(font);
    fbDoc.SetOwnerWindow(this);
    fbDoc.SetRenderer(r);
    m_panels.push_back(std::move(fbDoc));
    m_slots.push_back({ EPanelColumn::Left, 0 });

    // ── Camera ────────────────────────────────
    auto camPtr = std::make_unique<IECameraPanel>();
    m_camPanel  = camPtr.get();
    camPtr->SetCamera(m_vpPanel->GetCamera());

    auto camDoc = IEDockedPanel(std::move(camPtr));
    camDoc.SetFont(font);
    camDoc.SetOwnerWindow(this);
    camDoc.SetRenderer(r);
    m_panels.push_back(std::move(camDoc));
    m_slots.push_back({ EPanelColumn::Left, 1 });

    // ── EntityList ────────────────────────────
    auto elPtr    = std::make_unique<IEHierarchy>();
    m_entityPanel = elPtr.get();
    elPtr->SetScene(&m_vpPanel->GetScene());

    auto elDoc = IEDockedPanel(std::move(elPtr));
    elDoc.SetFont(font);
    elDoc.SetOwnerWindow(this);
    elDoc.SetRenderer(r);
    m_panels.push_back(std::move(elDoc));
    m_slots.push_back({ EPanelColumn::Right, 0 });

    // ── Inspector ─────────────────────────────
    auto inspPtr  = std::make_unique<IEInspectorPanel>();
    m_inspPanel   = inspPtr.get();

    auto inspDoc = IEDockedPanel(std::move(inspPtr));
    inspDoc.SetFont(font);
    inspDoc.SetOwnerWindow(this);
    inspDoc.SetRenderer(r);
    m_panels.push_back(std::move(inspDoc));
    m_slots.push_back({ EPanelColumn::Right, 1 });
}

void IEMainEditorWindow::LayoutPanels()
{
    const int32_t winW  = GetWidth();
    const int32_t bodyH = GetHeight() - kMenuH;

    // 열별로 살아있는 패널 수집
    struct ColEntry { int32_t idx; int32_t order; };
    std::vector<ColEntry> leftCol, centerCol, rightCol;

    for (int32_t i = 0; i < static_cast<int32_t>(m_panels.size()); ++i)
    {
        if (!m_panels[i].IsAlive())
            continue;
        ColEntry e{ i, m_slots[i].order };
        switch (m_slots[i].column)
        {
            case EPanelColumn::Left:   leftCol.push_back(e);   break;
            case EPanelColumn::Center: centerCol.push_back(e); break;
            case EPanelColumn::Right:  rightCol.push_back(e);  break;
        }
    }

    auto byOrder = [](const ColEntry& a, const ColEntry& b){ return a.order < b.order; };
    std::sort(leftCol.begin(),   leftCol.end(),   byOrder);
    std::sort(centerCol.begin(), centerCol.end(), byOrder);
    std::sort(rightCol.begin(),  rightCol.end(),  byOrder);

    // 열이 비면 폭 0 → 인접 열 자동 확장
    int32_t leftW   = leftCol.empty()   ? 0 : m_leftColW;
    int32_t rightW  = rightCol.empty()  ? 0 : m_rightColW;
    int32_t centerW = winW - leftW - rightW;
    int32_t rightX  = winW - rightW;

    // 비율 기반 행 높이 분배 (2패널: sepRatio 사용, 그 외 균등)
    auto assignCol = [&](std::vector<ColEntry>& col, int32_t colX, int32_t colW, float sepRatio)
    {
        if (col.empty())
            return;
        int32_t n = static_cast<int32_t>(col.size());
        if (n == 1)
        {
            m_panels[col[0].idx].SetRect(colX, kMenuH, colW, bodyH);
        }
        else if (n == 2)
        {
            int32_t h0 = std::max(20, static_cast<int32_t>(bodyH * sepRatio));
            int32_t h1 = bodyH - h0;
            m_panels[col[0].idx].SetRect(colX, kMenuH,      colW, h0);
            m_panels[col[1].idx].SetRect(colX, kMenuH + h0, colW, h1);
        }
        else
        {
            int32_t baseH = bodyH / n;
            for (int32_t j = 0; j < n; ++j)
            {
                int32_t h = (j == n - 1) ? bodyH - j * baseH : baseH;
                m_panels[col[j].idx].SetRect(colX, kMenuH + j * baseH, colW, h);
            }
        }
    };

    assignCol(leftCol,   0,      leftW,   m_leftSepRatio);
    assignCol(centerCol, leftW,  centerW, 0.5f);
    assignCol(rightCol,  rightX, rightW,  m_rightSepRatio);
}

// ─────────────────────────────────────────
// Resize
// ─────────────────────────────────────────

void IEMainEditorWindow::OnResize(int32_t /*w*/, int32_t /*h*/)
{
    LayoutPanels();
}

// ─────────────────────────────────────────
// CallXButton
// ─────────────────────────────────────────

void IEMainEditorWindow::CallXButton()
{
    IECore::StopEngine();
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

    // Ctrl+Z : Undo
    bool zDown = input.GetKeyState(SDL_SCANCODE_Z);
    if (ctrlHeld && zDown && !m_prevCtrlZ)
        m_history.Undo();
    m_prevCtrlZ = ctrlHeld && zDown;

    // Ctrl+Y : Redo
    bool yDown = input.GetKeyState(SDL_SCANCODE_Y);
    if (ctrlHeld && yDown && !m_prevCtrlY)
        m_history.Redo();
    m_prevCtrlY = ctrlHeld && yDown;

    // Delete : 선택된 오브젝트 삭제
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
                    if (objs[i].get() == vpSel)
                    {
                        idx = i;
                        break;
                    }
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

    // 역순으로 업데이트 (최상위 z-order 패널이 입력 우선)
    for (int32_t i = static_cast<int32_t>(m_panels.size()) - 1; i >= 0; --i)
    {
        if (m_panels[i].IsAlive())
            m_panels[i].Update(deltaTime);
    }

    // 타이틀바 클릭 → z-order 갱신 (back = 최상위)
    for (int32_t i = 0; i < static_cast<int32_t>(m_panels.size()) - 1; ++i)
    {
        if (m_panels[i].WasTitleClicked())
        {
            // 클릭된 패널을 맨 뒤(최상위)로 이동
            for (int32_t j = i; j < static_cast<int32_t>(m_panels.size()) - 1; ++j)
            {
                std::swap(m_panels[j], m_panels[j + 1]);
                std::swap(m_slots[j],  m_slots[j + 1]);
            }
            break;
        }
    }

    ProcessUndock();

    // ── 구분선 드래그 ───────────────────────────────────────
    {
        float gxF = 0.0f, gyF = 0.0f;
        SDL_MouseButtonFlags btnState = SDL_GetGlobalMouseState(&gxF, &gyF);
        int32_t winX = 0, winY = 0;
        SDL_GetWindowPosition(GetSDLWindow(), &winX, &winY);
        int32_t mx = static_cast<int32_t>(gxF) - winX;
        int32_t my = static_cast<int32_t>(gyF) - winY;
        bool lmb      = (btnState & SDL_BUTTON_LMASK) != 0;
        bool lmbClick = lmb && !m_prevGlobalLMB;
        m_prevGlobalLMB = lmb;

        const int32_t winW  = GetWidth();
        const int32_t bodyH = GetHeight() - kMenuH;

        // 열 alive 여부 + 패널 수
        bool hasLeft = false, hasRight = false;
        int32_t leftPanelCount = 0, rightPanelCount = 0;
        for (auto& slot : m_slots)
        {
            if (slot.column == EPanelColumn::Left)  { hasLeft  = true; ++leftPanelCount;  }
            if (slot.column == EPanelColumn::Right) { hasRight = true; ++rightPanelCount; }
        }

        int32_t leftSepX  = hasLeft  ? m_leftColW          : -1;
        int32_t rightSepX = hasRight ? winW - m_rightColW   : -1;
        int32_t leftRowSepY  = (leftPanelCount  >= 2) ? kMenuH + static_cast<int32_t>(bodyH * m_leftSepRatio)  : -1;
        int32_t rightRowSepY = (rightPanelCount >= 2) ? kMenuH + static_cast<int32_t>(bodyH * m_rightSepRatio) : -1;

        if (!lmb)
        {
            m_sepDrag = ESepDrag::None;
        }
        else if (m_sepDrag == ESepDrag::None && lmbClick)
        {
            if (leftSepX >= 0 && mx >= leftSepX - kSepHitW && mx <= leftSepX + kSepHitW
                && my > kMenuH)
            {
                m_sepDrag = ESepDrag::LeftCol;
                m_sepDragStartMx  = mx;
                m_sepDragStartVal = m_leftColW;
            }
            else if (rightSepX >= 0 && mx >= rightSepX - kSepHitW && mx <= rightSepX + kSepHitW
                     && my > kMenuH)
            {
                m_sepDrag = ESepDrag::RightCol;
                m_sepDragStartMx  = mx;
                m_sepDragStartVal = m_rightColW;
            }
            else if (leftRowSepY >= 0 && my >= leftRowSepY - kSepHitW && my <= leftRowSepY + kSepHitW
                     && mx >= 0 && mx < m_leftColW)
            {
                m_sepDrag = ESepDrag::LeftRow;
                m_sepDragStartMx    = my;
                m_sepDragStartRatio = m_leftSepRatio;
            }
            else if (rightRowSepY >= 0 && my >= rightRowSepY - kSepHitW && my <= rightRowSepY + kSepHitW
                     && mx >= winW - m_rightColW && mx < winW)
            {
                m_sepDrag = ESepDrag::RightRow;
                m_sepDragStartMx    = my;
                m_sepDragStartRatio = m_rightSepRatio;
            }
        }

        bool needLayout = false;
        if (m_sepDrag == ESepDrag::LeftCol)
        {
            int32_t newW = m_sepDragStartVal + (mx - m_sepDragStartMx);
            int32_t clamped = std::max(kMinColW, std::min(newW, winW - m_rightColW - kMinColW));
            if (clamped != m_leftColW) { m_leftColW = clamped; needLayout = true; }
        }
        else if (m_sepDrag == ESepDrag::RightCol)
        {
            int32_t newW = m_sepDragStartVal - (mx - m_sepDragStartMx);
            int32_t clamped = std::max(kMinColW, std::min(newW, winW - m_leftColW - kMinColW));
            if (clamped != m_rightColW) { m_rightColW = clamped; needLayout = true; }
        }
        else if (m_sepDrag == ESepDrag::LeftRow)
        {
            float newR = m_sepDragStartRatio + static_cast<float>(my - m_sepDragStartMx) / bodyH;
            float clamped = std::max(0.1f, std::min(newR, 0.9f));
            if (clamped != m_leftSepRatio) { m_leftSepRatio = clamped; needLayout = true; }
        }
        else if (m_sepDrag == ESepDrag::RightRow)
        {
            float newR = m_sepDragStartRatio + static_cast<float>(my - m_sepDragStartMx) / bodyH;
            float clamped = std::max(0.1f, std::min(newR, 0.9f));
            if (clamped != m_rightSepRatio) { m_rightSepRatio = clamped; needLayout = true; }
        }
        if (needLayout)
            LayoutPanels();
    }
}

void IEMainEditorWindow::ProcessUndock()
{
    for (int32_t i = 0; i < static_cast<int32_t>(m_panels.size()); ++i)
    {
        if (!m_panels[i].ShouldUndock())
            continue;

        m_panels[i].ClearUndockFlag();

        int32_t fx = m_panels[i].GetUndockScreenX();
        int32_t fy = m_panels[i].GetUndockScreenY();
        int32_t fw = m_panels[i].GetUndockW();
        int32_t fh = m_panels[i].GetUndockH();
        PanelSlot origSlot = m_slots[i]; // erase 전에 저장

        auto panelPtr = m_panels[i].ReleasePanel();
        m_panels.erase(m_panels.begin() + i);
        m_slots.erase(m_slots.begin() + i);

        // raw 포인터 무효화 확인 (소유권 이전된 패널이면 nullptr로)
        if (m_vpPanel != nullptr && m_vpPanel == dynamic_cast<IEViewportPanel*>(panelPtr.get()))
            m_vpPanel = nullptr;
        if (m_camPanel != nullptr && m_camPanel == dynamic_cast<IECameraPanel*>(panelPtr.get()))
            m_camPanel = nullptr;
        if (m_entityPanel != nullptr && m_entityPanel == dynamic_cast<IEHierarchy*>(panelPtr.get()))
            m_entityPanel = nullptr;
        if (m_inspPanel != nullptr && m_inspPanel == dynamic_cast<IEInspectorPanel*>(panelPtr.get()))
            m_inspPanel = nullptr;

        // 남은 패널들이 빈 공간을 채우도록 레이아웃 재계산
        LayoutPanels();

        // 메인 스레드에서 SDL 창 생성 (HWND 스레드 어피니티 → 커서/메시지 큐 정상 처리)
        std::string winId = "float_" + std::to_string(m_floatIdCounter.fetch_add(1));

        struct CreateTask
        {
            std::unique_ptr<IEPanel> panel;
            std::string              title;
            int32_t                  fw, fh, fx, fy;
            IEFont*                  font;
            std::string              windowId;
            PanelSlot                slot;
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
        task->slot     = origSlot;

        IECore::PostMainThreadTask([task, this]() {
            auto* floatWin = new IEFloatingPanelWindow();
            floatWin->CreateWindow(task->title.c_str(), task->fw, task->fh, task->fx, task->fy,
                static_cast<SDL_WindowFlags>(SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS));
            floatWin->Init(std::move(task->panel), task->font, task->windowId);
            EPanelColumn origCol = task->slot.column;
            floatWin->SetRedockCallback([this, origCol](std::unique_ptr<IEPanel> p) {
                Redock(std::move(p), origCol);
            });
            IECore::RequestAddWindow(task->windowId.c_str(), floatWin);
        });

        break; // 한 프레임에 하나만 처리
    }
}

void IEMainEditorWindow::RescanRawPtrs()
{
    m_vpPanel     = nullptr;
    m_camPanel    = nullptr;
    m_entityPanel = nullptr;
    m_inspPanel   = nullptr;
    for (auto& dp : m_panels)
    {
        if (!dp.IsAlive())
            continue;
        IEPanel* p = dp.GetPanel();
        if (auto* vp = dynamic_cast<IEViewportPanel*>(p))   m_vpPanel     = vp;
        if (auto* cam = dynamic_cast<IECameraPanel*>(p))    m_camPanel    = cam;
        if (auto* hier = dynamic_cast<IEHierarchy*>(p))     m_entityPanel = hier;
        if (auto* insp = dynamic_cast<IEInspectorPanel*>(p)) m_inspPanel  = insp;
    }
}

void IEMainEditorWindow::Redock(std::unique_ptr<IEPanel> panel, EPanelColumn col)
{
    // 해당 열의 현재 최대 order + 1 에 삽입
    int32_t maxOrder = -1;
    for (auto& slot : m_slots)
    {
        if (slot.column == col)
            maxOrder = std::max(maxOrder, slot.order);
    }

    auto doc = IEDockedPanel(std::move(panel));
    doc.SetFont(m_font);
    doc.SetOwnerWindow(this);
    doc.SetRenderer(GetRenderer(0));

    m_panels.push_back(std::move(doc));
    m_slots.push_back({ col, maxOrder + 1 });

    RescanRawPtrs();
    LayoutPanels();
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

    // 배경
    r->DrawRect(kColBg,   0, 0,      winW, winH,   SDL_BLENDMODE_NONE);
    r->DrawRect(kColMenu, 0, 0,      winW, kMenuH, SDL_BLENDMODE_NONE);
    r->DrawLine(kColSep,  0, kMenuH, winW, kMenuH);

    m_btnAtlas.Draw();
    m_btnSave.Draw();
    m_btnLoad.Draw();
    m_btnAddStatic.Draw();
    m_btnAddEntity.Draw();

    // 패널 (앞 → 뒤 순서로 그림, back이 최상위)
    for (auto& panel : m_panels)
    {
        if (panel.IsAlive())
            panel.Draw();
    }

    // 구분선
    {
        bool hasLeft = false, hasRight = false;
        int32_t leftPanelCount = 0, rightPanelCount = 0;
        for (auto& slot : m_slots)
        {
            if (slot.column == EPanelColumn::Left)  { hasLeft  = true; ++leftPanelCount;  }
            if (slot.column == EPanelColumn::Right) { hasRight = true; ++rightPanelCount; }
        }
        const int32_t bodyH = winH - kMenuH;

        auto colSepColor = [&](ESepDrag drag) {
            return (m_sepDrag == drag) ? kColSepHot : kColSep;
        };

        if (hasLeft)
            r->DrawLine(colSepColor(ESepDrag::LeftCol), m_leftColW, kMenuH, m_leftColW, winH);
        if (hasRight)
            r->DrawLine(colSepColor(ESepDrag::RightCol), winW - m_rightColW, kMenuH, winW - m_rightColW, winH);

        if (leftPanelCount >= 2)
        {
            int32_t y = kMenuH + static_cast<int32_t>(bodyH * m_leftSepRatio);
            r->DrawLine(colSepColor(ESepDrag::LeftRow), 0, y, m_leftColW, y);
        }
        if (rightPanelCount >= 2)
        {
            int32_t y = kMenuH + static_cast<int32_t>(bodyH * m_rightSepRatio);
            r->DrawLine(colSepColor(ESepDrag::RightRow), winW - m_rightColW, y, winW, y);
        }
    }
}
