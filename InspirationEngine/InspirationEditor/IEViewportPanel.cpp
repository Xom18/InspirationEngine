#include "IEViewportPanel.h"
#include <cstdio>
#include <algorithm>

SDL_Color IEViewportPanel::kColVp     = {  25,  25,  28, 255 };
SDL_Color IEViewportPanel::kColGrid   = {  48,  48,  52, 255 };
SDL_Color IEViewportPanel::kColOrigin = {  60, 100,  60, 255 };
SDL_Color IEViewportPanel::kColSel    = { 255, 200,  50, 255 };
SDL_Color IEViewportPanel::kColText   = { 120, 120, 120, 255 };

IEViewportPanel::IEViewportPanel()
{
    auto cam = std::make_unique<IECameraTopView>();
    m_camera = cam.get();
    m_scene.SetCamera(cam.release());
}

void IEViewportPanel::SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    m_x = x;
    m_y = y;
    m_w = (w > 0) ? w : 1;
    m_h = (h > 0) ? h : 1;
    if (m_camera != nullptr)
        m_camera->SetViewport(m_w, m_h);
}

void IEViewportPanel::Update(float dt)
{
    m_scene.Update(dt);
    UpdateInput();
}

void IEViewportPanel::UpdateInput()
{
    IEWindow* ownerWindow = GetOwnerWindow();
    if (m_camera == nullptr || ownerWindow == nullptr)
        return;
    if (IECore::GetMouseOnWindow() != ownerWindow)
    {
        m_vpPrevLMB  = false;
        m_vpPrevRMB  = false;
        m_vpDragging = false;
        m_objDragging = false;
        return;
    }

    float gx = 0.0f, gy = 0.0f;
    SDL_MouseButtonFlags btn = SDL_GetGlobalMouseState(&gx, &gy);

    int32_t winX = 0, winY = 0;
    SDL_GetWindowPosition(ownerWindow->GetSDLWindow(), &winX, &winY);

    int32_t mx = static_cast<int32_t>(gx) - winX;
    int32_t my = static_cast<int32_t>(gy) - winY;

    bool inVp = (mx >= m_x && mx < m_x + m_w && my >= m_y && my < m_y + m_h);

    int32_t vx = mx - m_x;
    int32_t vy = my - m_y;

    bool lmb        = (btn & SDL_BUTTON_LMASK) != 0;
    bool rmb        = (btn & SDL_BUTTON_RMASK) != 0;
    bool lmbClicked = lmb && !m_vpPrevLMB;
    bool rmbClicked = rmb && !m_vpPrevRMB;
    m_vpPrevLMB = lmb;
    m_vpPrevRMB = rmb;

    // G키: 그리드 표시 토글
    bool gDown = IECore::GetInput().GetKeyState(SDL_SCANCODE_G);
    if (gDown && !m_prevG)
        m_gridVisible = !m_gridVisible;
    m_prevG = gDown;

    if (inVp)
    {
        float wheelY = IECore::GetInput().GetMouseWheelY();
        if (wheelY != 0.0f)
        {
            float oldZoom = m_camera->GetZoom();
            float newZoom = std::clamp(oldZoom * std::pow(1.12f, wheelY), 0.05f, 20.0f);
            m_camera->SetViewport(m_w, m_h);
            auto wp = m_camera->ScreenToWorld(vx, vy);
            m_camera->SetZoom(newZoom);
            m_camera->SetViewport(m_w, m_h);
            auto wp2 = m_camera->ScreenToWorld(vx, vy);
            m_camera->SetPosition(
                m_camera->GetX() + static_cast<float>(wp.GetX() - wp2.GetX()),
                m_camera->GetY() + static_cast<float>(wp.GetY() - wp2.GetY()));
        }
    }

    // 카메라 패닝 (RMB)
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

    // 오브젝트 드래그 이동 (LMB)
    m_camera->SetViewport(m_w, m_h);
    auto curWp = m_camera->ScreenToWorld(vx, vy);
    float curWx = static_cast<float>(curWp.GetX());
    float curWy = static_cast<float>(curWp.GetY());

    if (lmbClicked && inVp && !m_vpDragging && m_selectedObj != nullptr)
    {
        auto* t = m_selectedObj->GetComponent<IETransformComponent>();
        if (t != nullptr)
        {
            float radius = 24.0f / m_camera->GetZoom();
            float dx = t->GetX() - curWx;
            float dy = t->GetY() - curWy;
            if (dx * dx + dy * dy <= radius * radius)
            {
                m_objDragging = true;
                m_objDragWx   = curWx;
                m_objDragWy   = curWy;
                m_objStartX   = t->GetX();
                m_objStartY   = t->GetY();
            }
        }
    }

    if (m_objDragging && m_selectedObj != nullptr)
    {
        auto* t = m_selectedObj->GetComponent<IETransformComponent>();
        if (t != nullptr)
        {
            t->SetX(m_objStartX + (curWx - m_objDragWx));
            t->SetY(m_objStartY + (curWy - m_objDragWy));
        }
    }

    if (!lmb && m_objDragging)
    {
        // 실제로 움직인 경우에만 커맨드 push
        if (m_selectedObj != nullptr && m_history != nullptr)
        {
            auto* t = m_selectedObj->GetComponent<IETransformComponent>();
            if (t != nullptr)
            {
                float newX = t->GetX();
                float newY = t->GetY();
                constexpr float kEps = 0.5f;
                if (std::abs(newX - m_objStartX) > kEps || std::abs(newY - m_objStartY) > kEps)
                {
                    // 이미 위치가 적용됐으므로 Execute 없이 히스토리만 push
                    // CmdMoveObject::Execute는 to로 세팅하므로 현재 위치 = toX/toY
                    auto cmd = std::make_unique<CmdMoveObject>(
                        m_selectedObj, m_objStartX, m_objStartY, newX, newY, &m_selectedObj);
                    // 직접 push하되 Execute는 건너뜀 (이미 적용됨)
                    m_history->PushNoExec(std::move(cmd));
                }
            }
        }
        m_objDragging = false;
    }

    // LMB 클릭 시 선택 (드래그 아닐 때)
    if (lmbClicked && inVp && !m_vpDragging && !m_objDragging)
        SelectAt(vx, vy);
}

void IEViewportPanel::AddObject(const std::string& type)
{
    if (m_camera == nullptr || m_history == nullptr)
        return;

    m_camera->SetViewport(m_w, m_h);
    auto center = m_camera->ScreenToWorld(m_w / 2, m_h / 2);
    float wx = static_cast<float>(center.GetX());
    float wy = static_cast<float>(center.GetY());

    m_history->Push(std::make_unique<CmdCreateObject>(&m_scene, type, wx, wy, &m_selectedObj));
}

void IEViewportPanel::DeleteSelectedObject()
{
    if (m_selectedObj == nullptr || m_history == nullptr)
        return;

    m_history->Push(std::make_unique<CmdDeleteObject>(&m_scene, m_selectedObj, &m_selectedObj));
}

void IEViewportPanel::SelectAt(int32_t vx, int32_t vy)
{
    m_camera->SetViewport(m_w, m_h);
    auto wp = m_camera->ScreenToWorld(vx, vy);
    float wx = static_cast<float>(wp.GetX());
    float wy = static_cast<float>(wp.GetY());

    float radius  = 24.0f / m_camera->GetZoom();
    IEGameObject* best     = nullptr;
    float         bestDist = radius;

    for (const auto& obj : m_scene.GetObjects())
    {
        if (!obj->IsActive())
            continue;
        auto* t = obj->GetComponent<IETransformComponent>();
        if (t == nullptr)
            continue;

        float dx   = t->GetX() - wx;
        float dy   = t->GetY() - wy;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist < bestDist)
        {
            bestDist = dist;
            best     = obj.get();
        }
    }
    m_selectedObj = best;
}

void IEViewportPanel::Draw(IERenderer* r)
{
    if (r == nullptr || m_camera == nullptr)
        return;

    SDL_Renderer* sdlR = r->GetSDLRenderer();
    SDL_Rect vpRect = { m_x, m_y, m_w, m_h };
    SDL_SetRenderViewport(sdlR, &vpRect);

    r->DrawRect(kColVp, 0, 0, m_w, m_h, SDL_BLENDMODE_NONE);

    m_camera->SetViewport(m_w, m_h);
    if (m_gridVisible)
        DrawGrid(r);

    m_scene.SetViewportOverride(m_w, m_h);
    m_scene.Draw(r);

    // 선택 표시
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

    IEFont* font = GetFont();
    if (font != nullptr)
    {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "cam (%.0f, %.0f)  zoom %.2f",
                      m_camera->GetX(), m_camera->GetY(), m_camera->GetZoom());
        r->DrawText(font, buf, kColText, 6, 6);

        int32_t objCount = static_cast<int32_t>(m_scene.GetObjects().size());
        std::snprintf(buf, sizeof(buf), "objects: %d", objCount);
        r->DrawText(font, buf, kColText, 6, m_h - 20);
    }

    SDL_SetRenderViewport(sdlR, nullptr);
}

void IEViewportPanel::DrawGrid(IERenderer* r)
{
    auto wTL = m_camera->ScreenToWorld(0, 0);
    auto wBR = m_camera->ScreenToWorld(m_w, m_h);

    float zoom     = m_camera->GetZoom();
    float gridSize = 64.0f;
    if (zoom < 0.25f)
        gridSize = 512.0f;
    else if (zoom < 0.5f)
        gridSize = 256.0f;
    else if (zoom > 4.0f)
        gridSize = 16.0f;
    else if (zoom > 2.0f)
        gridSize = 32.0f;

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

    auto orig = m_camera->WorldToScreen(0.0f, 0.0f);
    if (orig.GetX() >= -4 && orig.GetX() < m_w + 4 &&
        orig.GetY() >= -4 && orig.GetY() < m_h + 4)
    {
        r->DrawLine(kColOrigin, orig.GetX() - 10, orig.GetY(), orig.GetX() + 10, orig.GetY());
        r->DrawLine(kColOrigin, orig.GetX(), orig.GetY() - 10, orig.GetX(), orig.GetY() + 10);
    }
}
