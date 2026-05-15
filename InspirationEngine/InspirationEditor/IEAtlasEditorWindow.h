#pragma once
#include "../InspirationEngine/InspirationEngine.h"
#include <filesystem>
#include <vector>
#include <string>

class IEAtlasEditorWindow : public IEWindow
{
public:
    virtual void Draw() override;
    virtual void Update(float deltaTime) override;
    virtual void CallXButton() override;

    void InitWindow(IEFont* font);

private:
    struct TileDef
    {
        int32_t     x       = 0;
        int32_t     y       = 0;
        int32_t     w       = 0;
        int32_t     h       = 0;
        int32_t     pivotX  = 0;
        int32_t     pivotY  = 0;
        std::string name;
    };

    static constexpr int32_t kWinW    = 1024;
    static constexpr int32_t kWinH    = 720;
    static constexpr int32_t kHeaderH = 44;
    static constexpr int32_t kFooterH = 44;
    static constexpr int32_t kPanelW  = 260;
    static constexpr int32_t kCanvasX = 0;
    static constexpr int32_t kCanvasY = kHeaderH;
    static constexpr int32_t kCanvasW = kWinW - kPanelW;
    static constexpr int32_t kCanvasH = kWinH - kHeaderH - kFooterH;

    void LoadPNG();
    void AutoSplit();
    void SaveJSON();
    void ApplyPanel();
    void OnClick(int32_t mx, int32_t my);
    void FillPanelFromSelected();
    void SetFocus(IETextBox* tb);
    bool HitTest(const SDL_Rect& r, int32_t x, int32_t y) const;

    IEFont* m_font = nullptr;

    SDL_Surface* m_surface = nullptr;
    SDL_Texture* m_texture = nullptr;
    int32_t      m_imgW    = 0;
    int32_t      m_imgH    = 0;
    int32_t      m_tileW   = 32;
    int32_t      m_tileH   = 16;

    int32_t m_canvasDrawX = 0;
    int32_t m_canvasDrawY = 0;
    int32_t m_canvasDrawW = 0;
    int32_t m_canvasDrawH = 0;

    std::vector<TileDef> m_tiles;
    int32_t              m_selectedTile = -1;

    IETextBox m_tbPath;
    IETextBox m_tbTileW;
    IETextBox m_tbTileH;
    IETextBox m_tbName;
    IETextBox m_tbX;
    IETextBox m_tbY;
    IETextBox m_tbTW;
    IETextBox m_tbTH;
    IETextBox m_tbPivotX;
    IETextBox m_tbPivotY;

    IETextBox* m_focusedBox = nullptr;

    IEButton m_btnLoad;
    IEButton m_btnApply;
    IEButton m_btnSave;

    bool        m_prevLMB  = false;
    std::string m_statusMsg;
};
