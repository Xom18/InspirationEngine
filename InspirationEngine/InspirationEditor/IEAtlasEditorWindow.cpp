#include "IEAtlasEditorWindow.h"
#include <nlohmann/json.hpp>
#include <SDL3_image/SDL_image.h>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

static SDL_Color kColBg      = { 40,  40,  40, 255 };
static SDL_Color kColHeader  = { 55,  55,  60, 255 };
static SDL_Color kColPanel   = { 50,  50,  55, 255 };
static SDL_Color kColFooter  = { 55,  55,  60, 255 };
static SDL_Color kColGrid    = { 80,  80,  80, 255 };
static SDL_Color kColSel     = { 220,  50,  50, 255 };
static SDL_Color kColBtn     = { 70,  90, 120, 255 };
static SDL_Color kColBtnHov  = { 90, 115, 150, 255 };
static SDL_Color kColText    = { 220, 220, 220, 255 };
static SDL_Color kColTbBg    = { 30,  30,  35, 255 };
static SDL_Color kColTbBor   = { 90,  90,  95, 255 };
static SDL_Color kColTbFoc   = { 70, 130, 180, 255 };

void IEAtlasEditorWindow::InitWindow(IEFont* font)
{
    m_font = font;

    IERenderer* r = GetRenderer(0);

    // Header text boxes
    m_tbPath.SetFont(font);
    m_tbPath.SetRenderer(r);
    m_tbPath.SetRect(60, 8, 480, 28);
    m_tbPath.SetStyle(dTEXT_BOX_STYLE_EDITABLE);
    m_tbPath.SetDefaultColor(kColText);
    m_tbPath.SetCursorPos(0);

    m_tbTileW.SetFont(font);
    m_tbTileW.SetRenderer(r);
    m_tbTileW.SetRect(620, 8, 60, 28);
    m_tbTileW.SetStyle(dTEXT_BOX_STYLE_EDITABLE);
    m_tbTileW.SetDefaultColor(kColText);
    m_tbTileW.SetText("32");
    m_tbTileW.SetCursorPos(0);

    m_tbTileH.SetFont(font);
    m_tbTileH.SetRenderer(r);
    m_tbTileH.SetRect(720, 8, 60, 28);
    m_tbTileH.SetStyle(dTEXT_BOX_STYLE_EDITABLE);
    m_tbTileH.SetDefaultColor(kColText);
    m_tbTileH.SetText("16");
    m_tbTileH.SetCursorPos(0);

    // Panel text boxes (x offset = kCanvasW + 8)
    int32_t px = kCanvasW + 8;
    int32_t py = kHeaderH + 8;
    int32_t pw = kPanelW - 16;
    int32_t ph = 22;
    int32_t gap = 28;

    m_tbName.SetFont(font);
    m_tbName.SetRenderer(r);
    m_tbName.SetRect(px, py, pw, ph);
    m_tbName.SetStyle(dTEXT_BOX_STYLE_EDITABLE);
    m_tbName.SetDefaultColor(kColText);
    m_tbName.SetCursorPos(0);
    py += gap;

    m_tbX.SetFont(font);
    m_tbX.SetRenderer(r);
    m_tbX.SetRect(px + 20, py, pw - 20, ph);
    m_tbX.SetStyle(dTEXT_BOX_STYLE_EDITABLE);
    m_tbX.SetDefaultColor(kColText);
    m_tbX.SetCursorPos(0);
    py += gap;

    m_tbY.SetFont(font);
    m_tbY.SetRenderer(r);
    m_tbY.SetRect(px + 20, py, pw - 20, ph);
    m_tbY.SetStyle(dTEXT_BOX_STYLE_EDITABLE);
    m_tbY.SetDefaultColor(kColText);
    m_tbY.SetCursorPos(0);
    py += gap;

    m_tbTW.SetFont(font);
    m_tbTW.SetRenderer(r);
    m_tbTW.SetRect(px + 20, py, pw - 20, ph);
    m_tbTW.SetStyle(dTEXT_BOX_STYLE_EDITABLE);
    m_tbTW.SetDefaultColor(kColText);
    m_tbTW.SetCursorPos(0);
    py += gap;

    m_tbTH.SetFont(font);
    m_tbTH.SetRenderer(r);
    m_tbTH.SetRect(px + 20, py, pw - 20, ph);
    m_tbTH.SetStyle(dTEXT_BOX_STYLE_EDITABLE);
    m_tbTH.SetDefaultColor(kColText);
    m_tbTH.SetCursorPos(0);
    py += gap;

    m_tbPivotX.SetFont(font);
    m_tbPivotX.SetRenderer(r);
    m_tbPivotX.SetRect(px + 50, py, pw - 50, ph);
    m_tbPivotX.SetStyle(dTEXT_BOX_STYLE_EDITABLE);
    m_tbPivotX.SetDefaultColor(kColText);
    m_tbPivotX.SetCursorPos(0);
    py += gap;

    m_tbPivotY.SetFont(font);
    m_tbPivotY.SetRenderer(r);
    m_tbPivotY.SetRect(px + 50, py, pw - 50, ph);
    m_tbPivotY.SetStyle(dTEXT_BOX_STYLE_EDITABLE);
    m_tbPivotY.SetDefaultColor(kColText);
    m_tbPivotY.SetCursorPos(0);

    // Buttons
    m_btnLoad  = { 790, 8,  80, 28 };
    m_btnApply = { kCanvasW + 8, kWinH - kFooterH - 36, kPanelW - 16, 28 };
    m_btnSave  = { 8, kWinH - kFooterH + 8, 120, 28 };
}

void IEAtlasEditorWindow::CallXButton()
{
    HideWindow();
}

bool IEAtlasEditorWindow::HitTest(const SDL_Rect& r, int32_t x, int32_t y) const
{
    return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}

void IEAtlasEditorWindow::DrawBtn(IERenderer* r, const SDL_Rect& rect, const char* label)
{
    r->DrawRect(kColBtn, rect.x, rect.y, rect.w, rect.h, SDL_BLENDMODE_NONE);
    r->DrawText(m_font, label, kColText, rect.x + 4, rect.y + 4);
}

static void DrawTextBoxBg(IERenderer* r, IETextBox* tb, bool focused, SDL_Color colBor)
{
    SDL_Rect rect = {};
    tb->GetRect(rect);
    SDL_Color bor = focused ? SDL_Color{ 70, 130, 180, 255 } : colBor;
    r->DrawRect({ 30, 30, 35, 255 }, rect.x, rect.y, rect.w, rect.h, SDL_BLENDMODE_NONE);
    r->DrawLine(bor, rect.x,           rect.y,           rect.x + rect.w, rect.y);
    r->DrawLine(bor, rect.x,           rect.y + rect.h,  rect.x + rect.w, rect.y + rect.h);
    r->DrawLine(bor, rect.x,           rect.y,           rect.x,           rect.y + rect.h);
    r->DrawLine(bor, rect.x + rect.w,  rect.y,           rect.x + rect.w,  rect.y + rect.h);
}

void IEAtlasEditorWindow::Draw()
{
    IERenderer* r = GetRenderer(0);
    if (r == nullptr)
        return;

    SDL_Renderer* sdlR = r->GetSDLRenderer();

    // Background
    r->DrawRect(kColBg, 0, 0, kWinW, kWinH, SDL_BLENDMODE_NONE);

    // Header
    r->DrawRect(kColHeader, 0, 0, kWinW, kHeaderH, SDL_BLENDMODE_NONE);

    // Panel
    r->DrawRect(kColPanel, kCanvasW, 0, kPanelW, kWinH, SDL_BLENDMODE_NONE);

    // Footer
    r->DrawRect(kColFooter, 0, kWinH - kFooterH, kWinW, kFooterH, SDL_BLENDMODE_NONE);

    // Header labels
    r->DrawText(m_font, "PNG:", kColText, 4, 12);
    r->DrawText(m_font, "W:", kColText, 594, 12);
    r->DrawText(m_font, "H:", kColText, 694, 12);

    // Header text boxes
    DrawTextBoxBg(r, &m_tbPath, m_focusedBox == &m_tbPath, kColTbBor);
    m_tbPath.Draw();
    DrawTextBoxBg(r, &m_tbTileW, m_focusedBox == &m_tbTileW, kColTbBor);
    m_tbTileW.Draw();
    DrawTextBoxBg(r, &m_tbTileH, m_focusedBox == &m_tbTileH, kColTbBor);
    m_tbTileH.Draw();
    DrawBtn(r, m_btnLoad, "[Load]");

    // Canvas — PNG + grid + selection
    if (m_texture != nullptr && m_imgW > 0 && m_imgH > 0)
    {
        // Fit image into canvas area
        float scaleX = static_cast<float>(kCanvasW) / static_cast<float>(m_imgW);
        float scaleY = static_cast<float>(kCanvasH) / static_cast<float>(m_imgH);
        float scale  = (scaleX < scaleY) ? scaleX : scaleY;

        m_canvasDrawW = static_cast<int32_t>(m_imgW * scale);
        m_canvasDrawH = static_cast<int32_t>(m_imgH * scale);
        m_canvasDrawX = kCanvasX + (kCanvasW - m_canvasDrawW) / 2;
        m_canvasDrawY = kCanvasY + (kCanvasH - m_canvasDrawH) / 2;

        SDL_FRect dst = {
            static_cast<float>(m_canvasDrawX),
            static_cast<float>(m_canvasDrawY),
            static_cast<float>(m_canvasDrawW),
            static_cast<float>(m_canvasDrawH)
        };
        SDL_RenderTexture(sdlR, m_texture, nullptr, &dst);

        // Grid lines
        if (m_tileW > 0 && m_tileH > 0)
        {
            int32_t cols = m_imgW / m_tileW;
            int32_t rows = m_imgH / m_tileH;

            for (int32_t c = 0; c <= cols; ++c)
            {
                int32_t lx = m_canvasDrawX + static_cast<int32_t>(c * m_tileW * scale);
                r->DrawLine(kColGrid, lx, m_canvasDrawY, lx, m_canvasDrawY + m_canvasDrawH);
            }
            for (int32_t row = 0; row <= rows; ++row)
            {
                int32_t ly = m_canvasDrawY + static_cast<int32_t>(row * m_tileH * scale);
                r->DrawLine(kColGrid, m_canvasDrawX, ly, m_canvasDrawX + m_canvasDrawW, ly);
            }

            // Selected tile highlight
            if (m_selectedTile >= 0 && m_selectedTile < static_cast<int32_t>(m_tiles.size()))
            {
                const TileDef& td = m_tiles[m_selectedTile];
                int32_t sx = m_canvasDrawX + static_cast<int32_t>(td.x * scale);
                int32_t sy = m_canvasDrawY + static_cast<int32_t>(td.y * scale);
                int32_t sw = static_cast<int32_t>(td.w * scale);
                int32_t sh = static_cast<int32_t>(td.h * scale);
                r->DrawLine(kColSel, sx,      sy,      sx + sw, sy);
                r->DrawLine(kColSel, sx,      sy + sh, sx + sw, sy + sh);
                r->DrawLine(kColSel, sx,      sy,      sx,      sy + sh);
                r->DrawLine(kColSel, sx + sw, sy,      sx + sw, sy + sh);
            }
        }
    }

    // Panel labels + text boxes
    int32_t px = kCanvasW + 8;
    int32_t py = kHeaderH + 8;
    int32_t gap = 28;

    r->DrawText(m_font, "name:", kColText, px, py + 3);
    DrawTextBoxBg(r, &m_tbName, m_focusedBox == &m_tbName, kColTbBor);
    m_tbName.Draw();
    py += gap;

    r->DrawText(m_font, "x:", kColText, px, py + 3);
    DrawTextBoxBg(r, &m_tbX, m_focusedBox == &m_tbX, kColTbBor);
    m_tbX.Draw();
    py += gap;

    r->DrawText(m_font, "y:", kColText, px, py + 3);
    DrawTextBoxBg(r, &m_tbY, m_focusedBox == &m_tbY, kColTbBor);
    m_tbY.Draw();
    py += gap;

    r->DrawText(m_font, "w:", kColText, px, py + 3);
    DrawTextBoxBg(r, &m_tbTW, m_focusedBox == &m_tbTW, kColTbBor);
    m_tbTW.Draw();
    py += gap;

    r->DrawText(m_font, "h:", kColText, px, py + 3);
    DrawTextBoxBg(r, &m_tbTH, m_focusedBox == &m_tbTH, kColTbBor);
    m_tbTH.Draw();
    py += gap;

    r->DrawText(m_font, "pivX:", kColText, px, py + 3);
    DrawTextBoxBg(r, &m_tbPivotX, m_focusedBox == &m_tbPivotX, kColTbBor);
    m_tbPivotX.Draw();
    py += gap;

    r->DrawText(m_font, "pivY:", kColText, px, py + 3);
    DrawTextBoxBg(r, &m_tbPivotY, m_focusedBox == &m_tbPivotY, kColTbBor);
    m_tbPivotY.Draw();

    // Apply button
    DrawBtn(r, m_btnApply, "[Apply]");

    // Footer
    r->DrawText(m_font, m_statusMsg.c_str(), kColText, 150, kWinH - kFooterH + 12);
    DrawBtn(r, m_btnSave, "[Save JSON]");

    // Tile count
    std::string tileCount = "tiles: " + std::to_string(m_tiles.size());
    r->DrawText(m_font, tileCount.c_str(), kColText, 400, kWinH - kFooterH + 12);
}

void IEAtlasEditorWindow::Update(float /*deltaTime*/)
{
    if (IECore::GetMouseOnWindow() != this)
    {
        m_prevLMB = false;
        return;
    }

    float gx, gy;
    SDL_MouseButtonFlags btn = SDL_GetGlobalMouseState(&gx, &gy);

    int32_t winX = 0, winY = 0;
    SDL_GetWindowPosition(GetSDLWindow(), &winX, &winY);

    int32_t mx = static_cast<int32_t>(gx) - winX;
    int32_t my = static_cast<int32_t>(gy) - winY;

    bool lmb = (btn & SDL_BUTTON_LMASK) != 0;
    bool clicked = lmb && !m_prevLMB;
    m_prevLMB = lmb;

    if (clicked)
        OnClick(mx, my);
}

void IEAtlasEditorWindow::SetFocus(IETextBox* tb)
{
    m_focusedBox = tb;
    IECore::SetFocusedTextBox(tb);
}

void IEAtlasEditorWindow::OnClick(int32_t mx, int32_t my)
{
    // Header text boxes
    SDL_Rect rect = {};

    m_tbPath.GetRect(rect);
    if (HitTest(rect, mx, my)) { SetFocus(&m_tbPath); return; }

    m_tbTileW.GetRect(rect);
    if (HitTest(rect, mx, my)) { SetFocus(&m_tbTileW); return; }

    m_tbTileH.GetRect(rect);
    if (HitTest(rect, mx, my)) { SetFocus(&m_tbTileH); return; }

    // Load button
    if (HitTest(m_btnLoad, mx, my)) { LoadPNG(); return; }

    // Save button
    if (HitTest(m_btnSave, mx, my)) { SaveJSON(); return; }

    // Apply button
    if (HitTest(m_btnApply, mx, my)) { ApplyPanel(); return; }

    // Panel text boxes
    m_tbName.GetRect(rect);
    if (HitTest(rect, mx, my)) { SetFocus(&m_tbName); return; }

    m_tbX.GetRect(rect);
    if (HitTest(rect, mx, my)) { SetFocus(&m_tbX); return; }

    m_tbY.GetRect(rect);
    if (HitTest(rect, mx, my)) { SetFocus(&m_tbY); return; }

    m_tbTW.GetRect(rect);
    if (HitTest(rect, mx, my)) { SetFocus(&m_tbTW); return; }

    m_tbTH.GetRect(rect);
    if (HitTest(rect, mx, my)) { SetFocus(&m_tbTH); return; }

    m_tbPivotX.GetRect(rect);
    if (HitTest(rect, mx, my)) { SetFocus(&m_tbPivotX); return; }

    m_tbPivotY.GetRect(rect);
    if (HitTest(rect, mx, my)) { SetFocus(&m_tbPivotY); return; }

    // Canvas tile click
    if (mx >= kCanvasX && mx < kCanvasX + kCanvasW &&
        my >= kCanvasY && my < kCanvasY + kCanvasH &&
        m_texture != nullptr && m_canvasDrawW > 0 && m_canvasDrawH > 0)
    {
        float scaleX = static_cast<float>(m_canvasDrawW) / static_cast<float>(m_imgW);
        float scaleY = static_cast<float>(m_canvasDrawH) / static_cast<float>(m_imgH);

        int32_t imgPxX = static_cast<int32_t>((mx - m_canvasDrawX) / scaleX);
        int32_t imgPxY = static_cast<int32_t>((my - m_canvasDrawY) / scaleY);

        if (imgPxX >= 0 && imgPxX < m_imgW && imgPxY >= 0 && imgPxY < m_imgH &&
            m_tileW > 0 && m_tileH > 0)
        {
            int32_t col = imgPxX / m_tileW;
            int32_t row = imgPxY / m_tileH;
            int32_t cols = m_imgW / m_tileW;
            int32_t idx = row * cols + col;

            if (idx >= 0 && idx < static_cast<int32_t>(m_tiles.size()))
            {
                m_selectedTile = idx;
                FillPanelFromSelected();
            }
        }
        SetFocus(nullptr);
        return;
    }

    SetFocus(nullptr);
}

void IEAtlasEditorWindow::FillPanelFromSelected()
{
    if (m_selectedTile < 0 || m_selectedTile >= static_cast<int32_t>(m_tiles.size()))
        return;

    const TileDef& td = m_tiles[m_selectedTile];
    m_tbName.SetText(td.name.c_str());
    m_tbX.SetText(std::to_string(td.x).c_str());
    m_tbY.SetText(std::to_string(td.y).c_str());
    m_tbTW.SetText(std::to_string(td.w).c_str());
    m_tbTH.SetText(std::to_string(td.h).c_str());
    m_tbPivotX.SetText(std::to_string(td.pivotX).c_str());
    m_tbPivotY.SetText(std::to_string(td.pivotY).c_str());
}

void IEAtlasEditorWindow::ApplyPanel()
{
    if (m_selectedTile < 0 || m_selectedTile >= static_cast<int32_t>(m_tiles.size()))
        return;

    TileDef& td = m_tiles[m_selectedTile];
    td.name   = m_tbName.GetText();
    td.x      = std::atoi(m_tbX.GetText());
    td.y      = std::atoi(m_tbY.GetText());
    td.w      = std::atoi(m_tbTW.GetText());
    td.h      = std::atoi(m_tbTH.GetText());
    td.pivotX = std::atoi(m_tbPivotX.GetText());
    td.pivotY = std::atoi(m_tbPivotY.GetText());

    m_statusMsg = "Applied tile #" + std::to_string(m_selectedTile);
}

void IEAtlasEditorWindow::LoadPNG()
{
    std::string path = m_tbPath.GetText();
    if (path.empty())
    {
        m_statusMsg = "No path specified";
        return;
    }

    SDL_Surface* surf = IMG_Load(path.c_str());
    if (surf == nullptr)
    {
        m_statusMsg = std::string("Load failed: ") + SDL_GetError();
        return;
    }

    if (m_surface != nullptr)
        SDL_DestroySurface(m_surface);
    if (m_texture != nullptr)
        SDL_DestroyTexture(m_texture);

    m_surface = surf;
    m_imgW = surf->w;
    m_imgH = surf->h;

    SDL_Renderer* sdlR = GetRenderer(0)->GetSDLRenderer();
    m_texture = SDL_CreateTextureFromSurface(sdlR, surf);

    m_tileW = std::atoi(m_tbTileW.GetText());
    m_tileH = std::atoi(m_tbTileH.GetText());
    if (m_tileW <= 0) m_tileW = 32;
    if (m_tileH <= 0) m_tileH = 16;

    fs::path p(path);
    m_pngDir = p.parent_path().string();

    AutoSplit();
    m_statusMsg = "Loaded: " + p.filename().string();
}

void IEAtlasEditorWindow::AutoSplit()
{
    m_tiles.clear();
    m_selectedTile = -1;

    if (m_tileW <= 0 || m_tileH <= 0 || m_imgW <= 0 || m_imgH <= 0)
        return;

    int32_t cols = m_imgW / m_tileW;
    int32_t rows = m_imgH / m_tileH;
    int32_t defaultPivotX = m_tileW / 2;
    int32_t defaultPivotY = m_tileH / 2;

    for (int32_t row = 0; row < rows; ++row)
    {
        for (int32_t col = 0; col < cols; ++col)
        {
            TileDef td;
            td.x      = col * m_tileW;
            td.y      = row * m_tileH;
            td.w      = m_tileW;
            td.h      = m_tileH;
            td.pivotX = defaultPivotX;
            td.pivotY = defaultPivotY;
            td.name   = "tile_r" + std::to_string(row) + "_c" + std::to_string(col);
            m_tiles.push_back(td);
        }
    }
}

void IEAtlasEditorWindow::SaveJSON()
{
    if (m_tiles.empty())
    {
        m_statusMsg = "No tiles to save";
        return;
    }

    std::string pngPath = m_tbPath.GetText();
    fs::path p(pngPath);
    std::string outPath = (p.parent_path() / p.stem()).string() + ".json";

    nlohmann::json j;
    j["image"] = p.filename().string();
    j["tileStep"] = { {"x", m_tileW}, {"y", m_tileH} };

    int32_t defaultPivotX = m_tileW / 2;
    int32_t defaultPivotY = m_tileH / 2;
    if (!m_tiles.empty())
    {
        defaultPivotX = m_tiles[0].pivotX;
        defaultPivotY = m_tiles[0].pivotY;
    }
    j["defaultPivotX"] = defaultPivotX;
    j["defaultPivotY"] = defaultPivotY;

    nlohmann::json tiles;
    for (const TileDef& td : m_tiles)
    {
        tiles[td.name] = {
            {"x", td.x}, {"y", td.y},
            {"w", td.w}, {"h", td.h}
        };
    }
    j["tiles"] = tiles;

    std::ofstream ofs(outPath);
    if (!ofs)
    {
        m_statusMsg = "Cannot write: " + outPath;
        return;
    }
    ofs << j.dump(2);
    m_statusMsg = "Saved: " + fs::path(outPath).filename().string();
}
