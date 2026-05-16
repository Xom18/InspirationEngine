#pragma once
#include "IEEditorPanel.h"
#include "IEEditorCommand.h"
#include <cmath>

/// <summary>
/// 씬 뷰포트 패널. RMB 패닝 / 스크롤 줌 / LMB 선택 지원.
/// </summary>
class IEViewportPanel : public IEEditorPanel
{
public:
    /// <summary>
    /// 카메라 및 씬 초기화
    /// </summary>
    IEViewportPanel();

    /// <summary>
    /// 그리드, 오브젝트, 선택 표시를 렌더링
    /// </summary>
    /// <param name="r">사용할 렌더러</param>
    virtual void        Draw(IERenderer* r)                                override;

    /// <summary>
    /// 패닝·줌·선택 입력 처리 및 상태 갱신
    /// </summary>
    /// <param name="dt">경과 시간 (초)</param>
    virtual void        Update(float dt)                                   override;

    /// <summary>
    /// 패널 타이틀 문자열 반환
    /// </summary>
    virtual const char* GetTitle()                               const override { return IELocalize::Get("panel.viewport"); }

    /// <summary>
    /// 패널 콘텐츠 영역 설정
    /// </summary>
    /// <param name="x">좌상단 X</param>
    /// <param name="y">좌상단 Y</param>
    /// <param name="w">너비</param>
    /// <param name="h">높이</param>
    virtual void        SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h) override;

    /// <summary>
    /// 씬 참조 반환
    /// </summary>
    IEScene&         GetScene()          { return m_scene; }

    /// <summary>
    /// 뷰포트 카메라 포인터 반환
    /// </summary>
    IECameraTopView* GetCamera()         { return m_camera; }

    /// <summary>
    /// 현재 선택된 오브젝트 포인터 반환 (없으면 nullptr)
    /// </summary>
    IEGameObject*    GetSelectedObject() { return m_selectedObj; }

    /// <summary>
    /// 외부(Hierarchy 등)에서 선택 오브젝트 직접 지정
    /// </summary>
    void             SetSelectedObject(IEGameObject* obj) { m_selectedObj = obj; }

    /// <summary>
    /// 커맨드 히스토리 주입 (외부에서 Undo/Redo 공유)
    /// </summary>
    void SetCommandHistory(IECommandHistory* h) { m_history = h; }

    /// <summary>
    /// 그리드 표시 여부 설정 (프로젝트 설정 초기화용)
    /// </summary>
    void SetGridVisible(bool v) { m_gridVisible = v; }

    /// <summary>
    /// 뷰포트 중심 월드 좌표에 오브젝트 추가
    /// </summary>
    /// <param name="type">"StaticObject" 또는 "Entity"</param>
    void AddObject(const std::string& type);

    /// <summary>
    /// 현재 선택된 오브젝트 삭제
    /// </summary>
    void DeleteSelectedObject();

private:
    void DrawGrid(IERenderer* r);
    void UpdateInput();
    void SelectAt(int32_t vx, int32_t vy);

    static SDL_Color kColVp;
    static SDL_Color kColGrid;
    static SDL_Color kColOrigin;
    static SDL_Color kColSel;
    static SDL_Color kColText;

    IEScene          m_scene;
    IECameraTopView* m_camera      = nullptr;
    IEGameObject*    m_selectedObj = nullptr;

    int32_t m_x = 0, m_y = 0, m_w = 1, m_h = 1;

    bool  m_vpDragging   = false;
    float m_vpDragStartX = 0.0f;
    float m_vpDragStartY = 0.0f;
    float m_camStartX    = 0.0f;
    float m_camStartY    = 0.0f;
    bool  m_vpPrevLMB    = false;
    bool  m_vpPrevRMB    = false;
    bool  m_prevG        = false;
    bool  m_gridVisible  = true;

    // 오브젝트 드래그 이동
    bool  m_objDragging  = false;
    float m_objDragWx    = 0.0f;  // 드래그 시작 시 월드 마우스 X
    float m_objDragWy    = 0.0f;
    float m_objStartX    = 0.0f;  // 드래그 시작 시 오브젝트 위치
    float m_objStartY    = 0.0f;

    IECommandHistory* m_history = nullptr;
};
