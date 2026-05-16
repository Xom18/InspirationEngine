#pragma once
#include "../InspirationEngine/InspirationEngine.h"
#include "IEDockedPanel.h"
#include "IEEditorCommand.h"
#include "IEViewportPanel.h"
#include "IEFileBrowserPanel.h"
#include "IEHierarchy.h"
#include "IECameraPanel.h"
#include "IEInspectorPanel.h"
#include <list>
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
    // ── SplitNode 이진 트리 ───────────────────────────────────────────
    struct SplitNode
    {
        enum class Dir { H, V };  // H=좌우 분할, V=상하 분할

        bool    isLeaf    = true;
        Dir     dir       = Dir::H;
        float   ratio     = 0.5f;  // 첫 번째 자식(a)이 차지하는 비율

        IEDockedPanel*             dockedPanel = nullptr;  // isLeaf 전용
        std::unique_ptr<SplitNode> a, b;                   // branch 전용

        SDL_Rect rect{};  // LayoutTree 가 채움
        SDL_Rect sep{};   // 구분선 rect (branch 전용)
    };

    // ── 드롭 존 사이드 ────────────────────────────────────────────────
    enum class EDropSide { Center = 0, Left, Right, Top, Bottom };

    // ── 내부 메서드 ──────────────────────────────────────────────────
    void InitPanels(IEFont* font, IEAtlasEditorWindow* atlasEditor);
    void BuildLayoutTree();
    void LayoutPanels();
    void LayoutTree(SplitNode* node, SDL_Rect rect);
    void ProcessUndock();
    void Redock(std::unique_ptr<IEPanel> panel, IEDockedPanel* target, int32_t side);
    void RescanRawPtrs();

    void DrawSeps(SplitNode* node, IERenderer* r);
    void DrawDropZones(SplitNode* node, IERenderer* r, IEDockedPanel* target, EDropSide side);

    SplitNode* FindSepAt(SplitNode* node, int32_t mx, int32_t my);
    SplitNode* FindLeafAt(SplitNode* node, int32_t mx, int32_t my);
    SplitNode* FindAnyLeaf(SplitNode* node);

    void RemoveLeaf(IEDockedPanel* panel);
    void InsertLeaf(IEDockedPanel* newPanel, IEDockedPanel* target, EDropSide side);

    /// <summary>target leaf 를 찾아 SplitNode* 와 부모를 반환하는 헬퍼</summary>
    SplitNode* FindLeafNode(SplitNode* node, IEDockedPanel* panel);

    EDropSide  ComputeDropSide(SDL_Rect rect, int32_t mx, int32_t my);
    SDL_Rect   ComputeZoneRect(SDL_Rect rect, EDropSide side);

    // ── 상수 ────────────────────────────────────────────────────────
    static constexpr int32_t   kMenuH      = 40;
    static constexpr int32_t   kMinColW    = 60;
    static constexpr int32_t   kSepW       = 4;
    static constexpr int32_t   kSepHitW    = 5;
    static constexpr SDL_Color kColBg      = {  30,  30,  30, 255 };
    static constexpr SDL_Color kColMenu    = {  45,  45,  50, 255 };
    static constexpr SDL_Color kColSep     = {  65,  65,  70, 255 };
    static constexpr SDL_Color kColSepHot  = { 100, 130, 180, 255 };
    static constexpr SDL_Color kColDropHot = {  80, 180, 255, 140 };
    static constexpr SDL_Color kColText    = { 200, 200, 200, 255 };

    // ── 멤버 ────────────────────────────────────────────────────────
    IEAtlasEditorWindow* m_atlasEditor = nullptr;
    IEFont*              m_font        = nullptr;

    IEButton m_btnAtlas;
    IEButton m_btnSave;
    IEButton m_btnLoad;
    IEButton m_btnAddStatic;
    IEButton m_btnAddEntity;

    IECommandHistory m_history;

    static constexpr char kScenePath[] = "Data/scene.json";

    // 패널 소유 (list → 노드 주소 불변 → SplitNode::dockedPanel 포인터 안전)
    std::list<IEDockedPanel>   m_panels;
    std::unique_ptr<SplitNode> m_layoutRoot;

    // 빠른 접근용 raw 포인터 (소유권은 m_panels 안에 있음)
    IEViewportPanel*  m_vpPanel     = nullptr;
    IECameraPanel*    m_camPanel    = nullptr;
    IEHierarchy*      m_entityPanel = nullptr;
    IEInspectorPanel* m_inspPanel   = nullptr;

    // 부동 창 ID 카운터 (스레드 안전)
    std::atomic<int32_t> m_floatIdCounter{0};

    // 구분선 드래그 상태
    SplitNode* m_sepDragNode       = nullptr;
    float      m_sepDragStartRatio = 0.0f;
    int32_t    m_sepDragStartCoord = 0;  // H-sep: gx, V-sep: gy (글로벌)
    bool       m_prevGlobalLMB     = false;

    // Hierarchy ↔ Viewport 선택 동기화
    int32_t       m_prevHierIdx     = -1;
    IEGameObject* m_prevVpSelected  = nullptr;

    // 단축키 엣지 감지용 이전 상태
    bool m_prevCtrlS  = false;
    bool m_prevCtrlO  = false;
    bool m_prevCtrlZ  = false;
    bool m_prevCtrlY  = false;
    bool m_prevDelete = false;
};
