#pragma once
#include <string>
#include <vector>
#include <SDL3/SDL.h>

class IERenderer;
class IEFont;
class IEWindow;
class IEUIBase;

/// <summary>
/// 9방향 앵커: 패널 기준점(axis) 또는 위젯 기준점(pivot) 지정
/// </summary>
enum class IEAnchor : uint8_t
{
    TopLeft = 0, TopCenter, TopRight,
    MiddleLeft,  Center,    MiddleRight,
    BottomLeft,  BottomCenter, BottomRight
};

/// <summary>
/// 크기/위치 단위: 픽셀 또는 패널 크기 대비 비율(0.0–1.0)
/// </summary>
enum class IESizeMode : uint8_t { Pixel = 0, Ratio };

/// <summary>
/// JSON 기반 Anchor/Pivot UI 배치 시스템.
/// 패널이 이동/리사이즈될 때 SetContentRect()를 호출하면 모든 위젯 위치가 재계산된다.
/// </summary>
class IEPanelLayout
{
public:
    /// <summary>
    /// JSON 파일에서 레이아웃 정의를 로드
    /// </summary>
    bool LoadJson(const char* filePath);

    /// <summary>
    /// JSON에 정의된 name에 실제 위젯 포인터를 연결
    /// </summary>
    void Bind(const char* name, IEUIBase* widget);

    /// <summary>
    /// 코드로 위젯 추가 (JSON 없이 사용하는 폴백)
    /// </summary>
    void Add(IEUIBase* widget,
             IEAnchor axis, IEAnchor pivot,
             float x, float y, float w, float h,
             IESizeMode posMode  = IESizeMode::Pixel,
             IESizeMode sizeMode = IESizeMode::Pixel);

    /// <summary>
    /// 코드로 텍스트 라벨 추가
    /// </summary>
    void AddLabel(const char* text,
                  IEAnchor axis, IEAnchor pivot,
                  float x, float y,
                  SDL_Color color = { 180, 180, 180, 255 });

    /// <summary>
    /// 패널 콘텐츠 영역 설정 — 호출 시 모든 위젯 위치를 즉시 재계산
    /// </summary>
    void SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h);

    /// <summary>
    /// 모든 위젯에 폰트 전파
    /// </summary>
    void SetFont(IEFont* font);

    /// <summary>
    /// 모든 위젯에 소유 창 전파
    /// </summary>
    void SetOwnerWindow(IEWindow* win);

    /// <summary>
    /// 모든 위젯에 렌더러 전파
    /// </summary>
    void SetRenderer(IERenderer* renderer);

    void Draw();
    void Update();

    int32_t     GetOriginX()  const { return m_originX; }
    int32_t     GetOriginY()  const { return m_originY; }
    int32_t     GetPanelW()   const { return m_panelW; }
    int32_t     GetPanelH()   const { return m_panelH; }
    IEFont*     GetFont()     const { return m_font; }
    IERenderer* GetRenderer() const { return m_renderer; }

private:
    struct IELayoutElement
    {
        std::string name;
        IEAnchor    axis      = IEAnchor::TopLeft;
        IEAnchor    pivot     = IEAnchor::TopLeft;
        float       x         = 0.0f;
        float       y         = 0.0f;
        float       width     = 0.0f;
        float       height    = 0.0f;
        IESizeMode  posMode   = IESizeMode::Pixel;
        IESizeMode  sizeMode  = IESizeMode::Pixel;

        bool        isLabel   = false;
        std::string labelText;
        SDL_Color   labelColor = { 180, 180, 180, 255 };

        IEUIBase*   widget    = nullptr;
    };

    void       ApplyElement(const IELayoutElement& e) const;
    SDL_FPoint ResolveAnchorPoint(IEAnchor a, float ox, float oy, float w, float h) const;
    void       PropagateCommon(IEUIBase* widget) const;

    static IEAnchor   ParseAnchor(const std::string& s);
    static IESizeMode ParseMode(const std::string& s);

    std::vector<IELayoutElement> m_elements;
    int32_t     m_originX  = 0;
    int32_t     m_originY  = 0;
    int32_t     m_panelW   = 0;
    int32_t     m_panelH   = 0;
    IEFont*     m_font     = nullptr;
    IEWindow*   m_ownerWin = nullptr;
    IERenderer* m_renderer = nullptr;
};
