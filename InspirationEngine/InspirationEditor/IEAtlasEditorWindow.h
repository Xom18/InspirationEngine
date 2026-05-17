#pragma once
#include "../InspirationEngine/InspirationEngine.h"
#include <filesystem>
#include <vector>
#include <string>

class IEAtlasEditorWindow : public IEWindow
{
public:
    /// <summary>
    /// 창 전체 콘텐츠 렌더링
    /// </summary>
    virtual void Draw() override;

    /// <summary>
    /// 이벤트 처리 및 상태 갱신
    /// </summary>
    /// <param name="deltaTime">경과 시간 (초)</param>
    virtual void Update(float deltaTime) override;

    /// <summary>
    /// 닫기 버튼 동작 — 창을 숨김
    /// </summary>
    virtual void CallXButton() override;

    /// <summary>
    /// 폰트 및 위젯 초기화
    /// </summary>
    /// <param name="font">사용할 폰트</param>
    void InitWindow(IEFont* font);

    /// <summary>
    /// 지정 경로의 아틀라스 파일을 열어 로드
    /// </summary>
    /// <param name="path">파일 경로 (.png 또는 .json)</param>
    void OpenWithFile(const std::string& path);

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

    static constexpr int32_t kWinW     = 1280;
    static constexpr int32_t kWinH     = 720;
    static constexpr int32_t kHeaderH  = 44;
    static constexpr int32_t kFooterH  = 44;
    static constexpr int32_t kBrowserW = 200;
    static constexpr int32_t kPanelW   = 260;
    static constexpr int32_t kCanvasX  = kBrowserW;
    static constexpr int32_t kCanvasY  = kHeaderH;
    static constexpr int32_t kCanvasW  = kWinW - kBrowserW - kPanelW;
    static constexpr int32_t kCanvasH  = kWinH - kHeaderH - kFooterH;

    bool LoadPNGFromPath(const std::string& path);
    void LoadPNG();
    void LoadJSON(const std::string& path);
    void OnFileBrowserSelect(const std::string& path);
    void AutoSplit();
    void SaveJSON();
    void ApplyPanel();
    void OnClick(int32_t mx, int32_t my);
    void FillPanelFromSelected();
    void SetFocus(IEUITextBox* tb);
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

    // File browser panel
    IEUIFileBrowser m_fileBrowser;
    IEUITextBox     m_tbBrowserPath;
    IEUIButton      m_btnSetPath;

    // Atlas header
    IEUITextBox m_tbPath;
    IEUITextBox m_tbTileW;
    IEUITextBox m_tbTileH;

    // Atlas tile property panel
    IEUITextBox m_tbName;
    IEUITextBox m_tbX;
    IEUITextBox m_tbY;
    IEUITextBox m_tbTW;
    IEUITextBox m_tbTH;
    IEUITextBox m_tbPivotX;
    IEUITextBox m_tbPivotY;

    IEUITextBox* m_focusedBox = nullptr;

    IEUIButton m_btnLoad;
    IEUIButton m_btnApply;
    IEUIButton m_btnSave;

    bool        m_prevLMB  = false;
    std::string m_statusMsg;
};
