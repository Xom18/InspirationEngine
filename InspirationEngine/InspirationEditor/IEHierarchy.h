#pragma once
#include "IEEditorPanel.h"

/// <summary>
/// 씬 오브젝트 목록 패널. 클릭 시 선택 연동 예정.
/// </summary>
class IEHierarchy : public IEEditorPanel
{
public:
    /// <summary>
    /// 씬 오브젝트 목록 리스트뷰를 렌더링
    /// </summary>
    /// <param name="r">사용할 렌더러</param>
    virtual void        Draw(IERenderer* r)                                          override;

    /// <summary>
    /// 오브젝트 목록 동기화 및 입력 처리
    /// </summary>
    /// <param name="dt">경과 시간 (초)</param>
    virtual void        Update(float dt)                                             override;

    /// <summary>
    /// 패널 타이틀 문자열 반환
    /// </summary>
    virtual const char* GetTitle()                                         const override { return IELocalize::Get("panel.hierarchy"); }

    /// <summary>
    /// 패널 콘텐츠 영역 설정
    /// </summary>
    /// <param name="x">좌상단 X</param>
    /// <param name="y">좌상단 Y</param>
    /// <param name="w">너비</param>
    /// <param name="h">높이</param>
    virtual void        SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h) override;

    /// <summary>
    /// 폰트 설정 — 내부 위젯에 전파
    /// </summary>
    /// <param name="f">사용할 폰트</param>
    virtual void        SetFont(IEFont* f)                                           override;

    /// <summary>
    /// 소유 창 설정 — 내부 위젯에 전파
    /// </summary>
    /// <param name="w">소유 창 포인터</param>
    virtual void        SetOwnerWindow(IEWindow* w)                                  override;

    /// <summary>
    /// 렌더러 설정 — 내부 위젯에 전파
    /// </summary>
    /// <param name="r">사용할 렌더러</param>
    virtual void        SetRenderer(IERenderer* r)                                   override;

    /// <summary>
    /// 표시할 씬 참조 설정
    /// </summary>
    /// <param name="scene">대상 씬 포인터</param>
    void    SetScene(IEScene* scene) { m_scene = scene; }

    /// <summary>
    /// 현재 선택된 항목의 인덱스 반환 (없으면 -1)
    /// </summary>
    int32_t GetSelectedIndex() const { return m_list.GetSelectedIndex(); }

private:
    IEScene*   m_scene = nullptr;
    IEListView m_list;
};
