#include "IEMainEditorWindow.h"
#include "IEAtlasEditorWindow.h"
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cmath>

namespace fs = std::filesystem;

static SDL_Color kColBg      = {  30,  30,  30, 255 };
static SDL_Color kColMenu    = {  45,  45,  50, 255 };
static SDL_Color kColBrowser = {  40,  40,  45, 255 };
static SDL_Color kColVp      = {  25,  25,  28, 255 };
static SDL_Color kColSep     = {  65,  65,  70, 255 };
static SDL_Color kColText    = { 200, 200, 200, 255 };
static SDL_Color kColGrid    = {  48,  48,  52, 255 };
static SDL_Color kColOrigin  = {  60, 100,  60, 255 };
static SDL_Color kColSel     = { 255, 200,  50, 255 };

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
    m_btnAtlas.SetLabel("Atlas Editor");
    m_btnAtlas.SetOwnerWindow(this);
    m_btnAtlas.SetCallback([this]() {
        if (m_atlasEditor != nullptr)
            m_atlasEditor->ShowWindow();
    });

    constexpr int32_t kLabelH = 24;
    m_fileBrowser.SetRenderer(r);
    m_fileBrowser.SetFont(font);
    m_fileBrowser.SetOwnerWindow(this);
    m_fileBrowser.SetRect(0, kMenuH + kLabelH, kBrowserW, GetHeight() - kMenuH - kLabelH);
    m_fileBrowser.SetCallback([this](const std::string& path) { OnFileBrowserSelect(path); });

    InitScene();
}

void IEMainEditorWindow::InitScene()
{
    auto* cam = new IECameraTopView();
    cam->SetViewport(ViewportW(), ViewportH());
    m_camera = cam;
    m_scene.SetCamera(cam);
}

// ─────────────────────────────────────────
// Resize
// ─────────────────────────────────────────

void IEMainEditorWindow::OnResize(int32_t w, int32_t h)
{
    constexpr int32_t kLabelH = 24;
    m_fileBrowser.SetRect(0, kMenuH + kLabelH, kBrowserW, h - kMenuH - kLabelH);
}

// ─────────────────────────────────────────
// CallXButton
// ─────────────────────────────────────────

void IEMainEditorWindow::CallXButton()
{
    IECore::StopEngine();
}

// ─────────────────────────────────────────
// File browser callback
// ─────────────────────────────────────────

void IEMainEditorWindow::OnFileBrowserSelect(const std::string& path)
{
    if (m_atlasEditor == nullptr)
        return;

    std::string ext = fs::path(path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (ext == ".png" || ext == ".json")
        m_atlasEditor->OpenWithFile(path);
}

// ─────────────────────────────────────────
// Update
// ─────────────────────────────────────────

void IEMainEditorWindow::Update(float deltaTime)
{
    m_btnAtlas.Update();
    m_fileBrowser.Update();
    m_scene.Update(deltaTime);
    UpdateViewport();
}

void IEMainEditorWindow::UpdateViewport()
{
    if (m_camera == nullptr)
        return;
    if (IECore::GetMouseOnWindow() != this)
    {
        m_vpPrevLMB  = false;
        m_vpPrevRMB  = false;
        m_vpDragging = false;
        return;
    }

    const int32_t vpX = ViewportX();
    const int32_t vpY = ViewportY();
    const int32_t vpW = ViewportW();
    const int32_t vpH = ViewportH();

    float gx = 0.0f, gy = 0.0f;
    SDL_MouseButtonFlags btn = SDL_GetGlobalMouseState(&gx, &gy);

    int32_t winX = 0, winY = 0;
    SDL_GetWindowPosition(GetSDLWindow(), &winX, &winY);

    int32_t mx = static_cast<int32_t>(gx) - winX;
    int32_t my = static_cast<int32_t>(gy) - winY;

    bool inVp = (mx >= vpX && mx < vpX + vpW &&
                 my >= vpY && my < vpY + vpH);

    int32_t vx = mx - vpX;
    int32_t vy = my - vpY;

    bool lmb = (btn & SDL_BUTTON_LMASK) != 0;
    bool rmb = (btn & SDL_BUTTON_RMASK) != 0;
    bool lmbClicked = lmb && !m_vpPrevLMB;
    bool rmbClicked = rmb && !m_vpPrevRMB;
    m_vpPrevLMB = lmb;
    m_vpPrevRMB = rmb;

    // Scroll zoom — centered on mouse cursor
    if (inVp)
    {
        float wheelY = IECore::GetInput().GetMouseWheelY();
        if (wheelY != 0.0f)
        {
            float oldZoom = m_camera->GetZoom();
            float newZoom = std::clamp(oldZoom * std::pow(1.12f, wheelY), 0.05f, 20.0f);

            // Zoom toward cursor: shift camera so world point under cursor stays fixed
            m_camera->SetViewport(vpW, vpH);
            auto wp = m_camera->ScreenToWorld(vx, vy);
            m_camera->SetZoom(newZoom);
            m_camera->SetViewport(vpW, vpH);
            auto wp2 = m_camera->ScreenToWorld(vx, vy);
            m_camera->SetPosition(
                m_camera->GetX() + static_cast<float>(wp.GetX() - wp2.GetX()),
                m_camera->GetY() + static_cast<float>(wp.GetY() - wp2.GetY()));
        }
    }

    // RMB drag — pan
    if (rmbClicked && inVp)
    {
        m_vpDragging   = true;
        m_vpDragStartX = static_cast<float>(vx);
        m_vpDragStartY = static_cast<float>(vy);
        m_camStartX    = m_camera->GetX();
        m_camStartY    = m_camera->GetY();
    }
    if (!rmb)
        m_vpDragging = false;

    if (m_vpDragging)
    {
        float zoom = m_camera->GetZoom();
        float dx   = (static_cast<float>(vx) - m_vpDragStartX) / zoom;
        float dy   = (static_cast<float>(vy) - m_vpDragStartY) / zoom;
        m_camera->SetPosition(m_camStartX - dx, m_camStartY - dy);
    }

    // LMB click — select
    if (lmbClicked && inVp && !m_vpDragging)
        SelectAtViewportPos(vx, vy);
}

void IEMainEditorWindow::SelectAtViewportPos(int32_t vx, int32_t vy)
{
    m_camera->SetViewport(ViewportW(), ViewportH());
    auto wp = m_camera->ScreenToWorld(vx, vy);
    float wx = static_cast<float>(wp.GetX());
    float wy = static_cast<float>(wp.GetY());

    float radius = 24.0f / m_camera->GetZoom();
    IEGameObject* best = nullptr;
    float bestDist = radius;

    for (const auto& obj : m_scene.GetObjects())
    {
        if (!obj->IsActive()) continue;
        auto* t = obj->GetComponent<IETransformComponent>();
        if (t == nullptr) continue;

        float dx   = t->GetX() - wx;
        float dy   = t->GetY() - wy;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist < bestDist)
        {
            bestDist = dist;
            best = obj.get();
        }
    }

    m_selectedObj = best;
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

    // Backgrounds
    r->DrawRect(kColBg,      0,          0,       winW,     winH,             SDL_BLENDMODE_NONE);
    r->DrawRect(kColMenu,    0,          0,       winW,     kMenuH,           SDL_BLENDMODE_NONE);
    r->DrawRect(kColBrowser, 0,          kMenuH,  kBrowserW, winH - kMenuH,  SDL_BLENDMODE_NONE);

    // Separators
    r->DrawLine(kColSep, kBrowserW, 0,     kBrowserW, winH);
    r->DrawLine(kColSep, 0,         kMenuH, winW,     kMenuH);

    // Menu bar
    m_btnAtlas.Draw();

    // Resource browser header
    r->DrawText(m_font, "Resources", kColText, 6, kMenuH + 6);
    r->DrawLine(kColSep, 0, kMenuH + 22, kBrowserW, kMenuH + 22);
    m_fileBrowser.Draw();

    // Viewport
    DrawViewport();
}

void IEMainEditorWindow::DrawViewport()
{
    IERenderer*   r    = GetRenderer(0);
    SDL_Renderer* sdlR = r->GetSDLRenderer();

    const int32_t vpX = ViewportX();
    const int32_t vpY = ViewportY();
    const int32_t vpW = ViewportW();
    const int32_t vpH = ViewportH();

    SDL_Rect vpRect = { vpX, vpY, vpW, vpH };
    SDL_SetRenderViewport(sdlR, &vpRect);

    // Viewport background
    r->DrawRect(kColVp, 0, 0, vpW, vpH, SDL_BLENDMODE_NONE);

    if (m_camera != nullptr)
    {
        m_camera->SetViewport(vpW, vpH);
        DrawViewportGrid(r);

        m_scene.SetViewportOverride(vpW, vpH);
        m_scene.Draw(r);

        // Selection indicator
        if (m_selectedObj != nullptr)
        {
            auto* t = m_selectedObj->GetComponent<IETransformComponent>();
            if (t != nullptr)
            {
                auto sp = m_camera->WorldToScreen(t->GetX(), t->GetY(), t->GetZ());
                constexpr int32_t kR = 14;
                int32_t sx = sp.GetX();
                int32_t sy = sp.GetY();
                r->DrawLine(kColSel, sx - kR, sy - kR, sx + kR, sy - kR);
                r->DrawLine(kColSel, sx + kR, sy - kR, sx + kR, sy + kR);
                r->DrawLine(kColSel, sx + kR, sy + kR, sx - kR, sy + kR);
                r->DrawLine(kColSel, sx - kR, sy + kR, sx - kR, sy - kR);
            }
        }

        // Camera info overlay
        if (m_font != nullptr)
        {
            char buf[64];
            std::snprintf(buf, sizeof(buf), "cam (%.0f, %.0f)  zoom %.2f",
                          m_camera->GetX(), m_camera->GetY(), m_camera->GetZoom());
            r->DrawText(m_font, buf, { 120, 120, 120, 255 }, 6, 6);

            int32_t objCount = static_cast<int32_t>(m_scene.GetObjects().size());
            std::snprintf(buf, sizeof(buf), "objects: %d", objCount);
            r->DrawText(m_font, buf, { 120, 120, 120, 255 }, 6, vpH - 20);
        }
    }

    SDL_SetRenderViewport(sdlR, nullptr);
}

void IEMainEditorWindow::DrawViewportGrid(IERenderer* r)
{
    const int32_t vpW = ViewportW();
    const int32_t vpH = ViewportH();

    auto wTL = m_camera->ScreenToWorld(0, 0);
    auto wBR = m_camera->ScreenToWorld(vpW, vpH);

    float zoom     = m_camera->GetZoom();
    float gridSize = 64.0f;
    // Adapt grid spacing to zoom level
    if (zoom < 0.25f) gridSize = 512.0f;
    else if (zoom < 0.5f) gridSize = 256.0f;
    else if (zoom > 4.0f) gridSize = 16.0f;
    else if (zoom > 2.0f) gridSize = 32.0f;

    float startX = std::floor(static_cast<float>(wTL.GetX()) / gridSize) * gridSize;
    float startY = std::floor(static_cast<float>(wTL.GetY()) / gridSize) * gridSize;

    for (float wx = startX; wx <= static_cast<float>(wBR.GetX()) + gridSize; wx += gridSize)
    {
        auto s1 = m_camera->WorldToScreen(wx, static_cast<float>(wTL.GetY()));
        auto s2 = m_camera->WorldToScreen(wx, static_cast<float>(wBR.GetY()));
        r->DrawLine(kColGrid, s1.GetX(), s1.GetY(), s2.GetX(), s2.GetY());
    }
    for (float wy = startY; wy <= static_cast<float>(wBR.GetY()) + gridSize; wy += gridSize)
    {
        auto s1 = m_camera->WorldToScreen(static_cast<float>(wTL.GetX()), wy);
        auto s2 = m_camera->WorldToScreen(static_cast<float>(wBR.GetX()), wy);
        r->DrawLine(kColGrid, s1.GetX(), s1.GetY(), s2.GetX(), s2.GetY());
    }

    // World origin cross
    auto orig = m_camera->WorldToScreen(0.0f, 0.0f);
    if (orig.GetX() >= -4 && orig.GetX() < vpW + 4 &&
        orig.GetY() >= -4 && orig.GetY() < vpH + 4)
    {
        r->DrawLine(kColOrigin, orig.GetX() - 10, orig.GetY(), orig.GetX() + 10, orig.GetY());
        r->DrawLine(kColOrigin, orig.GetX(), orig.GetY() - 10, orig.GetX(), orig.GetY() + 10);
    }
}
