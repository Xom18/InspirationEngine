#include "IEMainEditorWindow.h"
#include "IEAtlasEditorWindow.h"
#include "IEFloatingPanelWindow.h"
#include <cstdio>

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

    InitPanels(font, atlasEditor);
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

    // ── FileBrowser ───────────────────────────
    auto fbPtr = std::make_unique<IEFileBrowserPanel>();
    fbPtr->SetAtlasEditor(atlasEditor);

    auto fbDoc = IEDockedPanel(std::move(fbPtr));
    fbDoc.SetFont(font);
    fbDoc.SetOwnerWindow(this);
    fbDoc.SetRenderer(r);
    m_panels.push_back(std::move(fbDoc));

    // ── Camera ────────────────────────────────
    auto camPtr = std::make_unique<IECameraPanel>();
    m_camPanel  = camPtr.get();
    camPtr->SetCamera(m_vpPanel->GetCamera());

    auto camDoc = IEDockedPanel(std::move(camPtr));
    camDoc.SetFont(font);
    camDoc.SetOwnerWindow(this);
    camDoc.SetRenderer(r);
    m_panels.push_back(std::move(camDoc));

    // ── EntityList ────────────────────────────
    auto elPtr    = std::make_unique<IEHierarchy>();
    m_entityPanel = elPtr.get();
    elPtr->SetScene(&m_vpPanel->GetScene());

    auto elDoc = IEDockedPanel(std::move(elPtr));
    elDoc.SetFont(font);
    elDoc.SetOwnerWindow(this);
    elDoc.SetRenderer(r);
    m_panels.push_back(std::move(elDoc));

    // ── Inspector ─────────────────────────────
    auto inspPtr  = std::make_unique<IEInspectorPanel>();
    m_inspPanel   = inspPtr.get();

    auto inspDoc = IEDockedPanel(std::move(inspPtr));
    inspDoc.SetFont(font);
    inspDoc.SetOwnerWindow(this);
    inspDoc.SetRenderer(r);
    m_panels.push_back(std::move(inspDoc));
}

void IEMainEditorWindow::LayoutPanels()
{
    const int32_t winW   = GetWidth();
    const int32_t winH   = GetHeight();
    const int32_t bodyH  = winH - kMenuH;

    // 좌측 폭 / 우측 폭 / 중앙 뷰포트 폭
    constexpr int32_t kLeftW  = 220;
    constexpr int32_t kRightW = 260;
    const int32_t     vpW     = winW - kLeftW - kRightW;

    // 좌측: FileBrowser(위) + Camera(아래)
    const int32_t fbH  = bodyH / 2;
    const int32_t camH = bodyH - fbH;

    // 우측: EntityList(위) + Inspector(아래)
    const int32_t elH   = bodyH / 2;
    const int32_t inspH = bodyH - elH;

    // m_panels 인덱스: 0=Viewport, 1=FileBrowser, 2=Camera, 3=EntityList, 4=Inspector
    if (m_panels.size() >= 5)
    {
        m_panels[0].SetRect(kLeftW,       kMenuH,           vpW,     bodyH); // Viewport
        m_panels[1].SetRect(0,            kMenuH,           kLeftW,  fbH);   // FileBrowser
        m_panels[2].SetRect(0,            kMenuH + fbH,     kLeftW,  camH);  // Camera
        m_panels[3].SetRect(kLeftW + vpW, kMenuH,           kRightW, elH);   // EntityList
        m_panels[4].SetRect(kLeftW + vpW, kMenuH + elH,     kRightW, inspH); // Inspector
    }
}

// ─────────────────────────────────────────
// Resize
// ─────────────────────────────────────────

void IEMainEditorWindow::OnResize(int32_t w, int32_t h)
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
                std::swap(m_panels[j], m_panels[j + 1]);
            // raw 포인터 재스캔 (이동 후 인덱스가 바뀔 수 있음)
            break;
        }
    }

    ProcessUndock();
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

        auto panelPtr = m_panels[i].ReleasePanel();
        m_panels.erase(m_panels.begin() + i);

        // raw 포인터 무효화 확인 (소유권 이전된 패널이면 nullptr로)
        if (m_vpPanel     && m_vpPanel     == dynamic_cast<IEViewportPanel*>(panelPtr.get()))    m_vpPanel     = nullptr;
        if (m_camPanel    && m_camPanel    == dynamic_cast<IECameraPanel*>(panelPtr.get()))       m_camPanel    = nullptr;
        if (m_entityPanel && m_entityPanel == dynamic_cast<IEHierarchy*>(panelPtr.get()))  m_entityPanel = nullptr;
        if (m_inspPanel   && m_inspPanel   == dynamic_cast<IEInspectorPanel*>(panelPtr.get()))   m_inspPanel   = nullptr;

        // 부동 창 생성
        std::string winId = "float_" + std::to_string(m_floatIdCounter.fetch_add(1));
        const char* title = panelPtr->GetTitle();

        auto* floatWin = new IEFloatingPanelWindow();
        floatWin->CreateWindow(title, fw, fh, fx, fy, SDL_WINDOW_RESIZABLE);
        floatWin->Init(std::move(panelPtr), m_font, winId);

        IECore::RequestAddWindow(winId.c_str(), floatWin);

        break; // 한 프레임에 하나만 처리
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

    // 배경
    r->DrawRect(kColBg,   0, 0,      winW, winH,   SDL_BLENDMODE_NONE);
    r->DrawRect(kColMenu, 0, 0,      winW, kMenuH, SDL_BLENDMODE_NONE);
    r->DrawLine(kColSep,  0, kMenuH, winW, kMenuH);

    m_btnAtlas.Draw();

    // 패널 (앞 → 뒤 순서로 그림, back이 최상위)
    for (auto& panel : m_panels)
    {
        if (panel.IsAlive())
            panel.Draw();
    }
}
