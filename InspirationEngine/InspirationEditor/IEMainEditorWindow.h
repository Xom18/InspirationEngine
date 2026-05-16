#pragma once
#include "../InspirationEngine/InspirationEngine.h"
#include "IEDockedPanel.h"
#include "IEEditorCommand.h"
#include "IEViewportPanel.h"
#include "IEFileBrowserPanel.h"
#include "IEHierarchy.h"
#include "IECameraPanel.h"
#include "IEInspectorPanel.h"
#include <vector>
#include <string>
#include <atomic>

class IEAtlasEditorWindow;

class IEMainEditorWindow : public IEWindow
{
public:
    /// <summary>
    /// 창 전체 콘텐츠 렌더링
    /// </summary>
    virtual void Draw()                          override;

    /// <summary>
    /// 이벤트 처리 및 패널 상태 갱신
    /// </summary>
    /// <param name="deltaTime">경과 시간 (초)</param>
    virtual void Update(float deltaTime)         override;

    /// <summary>
    /// 닫기 버튼 동작 — 엔진 종료
    /// </summary>
    virtual void CallXButton()                   override;

    /// <summary>
    /// 창 크기 변경 시 패널 레이아웃 재계산
    /// </summary>
    /// <param name="w">새 창 너비</param>
    /// <param name="h">새 창 높이</param>
    virtual void OnResize(int32_t w, int32_t h)  override;

    /// <summary>
    /// 패널 및 위젯 초기화
    /// </summary>
    /// <param name="font">사용할 폰트</param>
    /// <param name="atlasEditor">아틀라스 에디터 창 참조</param>
    void InitWindow(IEFont* font, IEAtlasEditorWindow* atlasEditor);

    static constexpr int32_t kMinW = 800;
    static constexpr int32_t kMinH = 500;

private:
    enum class EPanelColumn { Left, Center, Right };

    struct PanelSlot
    {
        EPanelColumn column;
        int32_t      order;   // 열 내 수직 순서 (0=상단)
    };

    void InitPanels(IEFont* font, IEAtlasEditorWindow* atlasEditor);
    void LayoutPanels();
    void ProcessUndock();
    void Redock(std::unique_ptr<IEPanel> panel, EPanelColumn col);
    void RescanRawPtrs();

    static constexpr int32_t   kMenuH      = 40;
    static constexpr int32_t   kMinColW    = 80;
    static constexpr int32_t   kSepHitW    = 5;   // separator hit half-width
    static constexpr SDL_Color kColBg      = {  30,  30,  30, 255 };
    static constexpr SDL_Color kColMenu    = {  45,  45,  50, 255 };
    static constexpr SDL_Color kColSep     = {  65,  65,  70, 255 };
    static constexpr SDL_Color kColSepHot  = { 100, 130, 180, 255 };
    static constexpr SDL_Color kColText    = { 200, 200, 200, 255 };

    IEAtlasEditorWindow* m_atlasEditor = nullptr;
    IEFont*              m_font        = nullptr;

    IEButton m_btnAtlas;
    IEButton m_btnSave;
    IEButton m_btnLoad;
    IEButton m_btnAddStatic;
    IEButton m_btnAddEntity;

    IECommandHistory m_history;

    static constexpr char kScenePath[] = "Data/scene.json";

    // 패널 리스트 (back = 최상위 z-order) + 열 메타데이터 (m_panels 와 1:1 대응)
    std::vector<IEDockedPanel> m_panels;
    std::vector<PanelSlot>     m_slots;

    // 빠른 접근용 raw 포인터 (소유권은 m_panels 안에 있음)
    IEViewportPanel*   m_vpPanel     = nullptr;
    IECameraPanel*     m_camPanel    = nullptr;
    IEHierarchy* m_entityPanel = nullptr;
    IEInspectorPanel*  m_inspPanel   = nullptr;

    // 부동 창 ID 카운터 (스레드 안전)
    std::atomic<int32_t> m_floatIdCounter{0};

    // 열 폭 (드래그로 조절 가능)
    int32_t m_leftColW  = 220;
    int32_t m_rightColW = 260;

    // 행 분할 비율 (좌/우 열에서 상단 패널이 차지하는 비율)
    float m_leftSepRatio  = 0.5f;
    float m_rightSepRatio = 0.5f;

    // 구분선 드래그 상태
    enum class ESepDrag { None, LeftCol, RightCol, LeftRow, RightRow };
    ESepDrag m_sepDrag           = ESepDrag::None;
    int32_t  m_sepDragStartMx    = 0;   // LeftCol/RightCol: x, LeftRow/RightRow: y
    int32_t  m_sepDragStartVal   = 0;   // 드래그 시작 시 열 폭
    float    m_sepDragStartRatio = 0.0f;
    bool     m_prevGlobalLMB     = false;

    // Hierarchy ↔ Viewport 선택 동기화
    int32_t      m_prevHierIdx      = -1;
    IEGameObject* m_prevVpSelected  = nullptr;

    // 단축키 엣지 감지용 이전 상태
    bool m_prevCtrlS  = false;
    bool m_prevCtrlO  = false;
    bool m_prevCtrlZ  = false;
    bool m_prevCtrlY  = false;
    bool m_prevDelete = false;
};
