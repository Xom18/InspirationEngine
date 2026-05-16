#pragma once
#include "IEEditorPanel.h"

class IEGameObject;

/// <summary>
/// 선택된 오브젝트의 Transform(X/Y/Z)을 편집하는 Inspector 패널.
/// </summary>
class IEInspectorPanel : public IEEditorPanel
{
public:
    /// <summary>
    /// 슬라이더 범위·콜백 및 레이아웃 초기화
    /// </summary>
    IEInspectorPanel();

    /// <summary>
    /// 선택 오브젝트의 Transform 슬라이더 렌더링
    /// </summary>
    /// <param name="r">사용할 렌더러</param>
    virtual void        Draw(IERenderer* r)                                          override;

    /// <summary>
    /// 대상 오브젝트 Transform과 슬라이더 동기화 및 입력 처리
    /// </summary>
    /// <param name="dt">경과 시간 (초)</param>
    virtual void        Update(float dt)                                             override;

    /// <summary>
    /// 패널 타이틀 문자열 반환
    /// </summary>
    virtual const char* GetTitle()                                         const override { return IELocalize::Get("panel.inspector"); }

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
    /// 편집 대상 오브젝트 설정 — 텍스트박스 값 즉시 동기화
    /// </summary>
    /// <param name="obj">대상 오브젝트 (nullptr 허용)</param>
    void SetTarget(IEGameObject* obj);

private:
    void SetFocus(IETextBox* tb);
    bool HitTest(const SDL_Rect& r, int32_t mx, int32_t my) const;
    void SyncTextToTransform(IETextBox& tb, float value);

    static constexpr SDL_Color kColBg    = {  35,  35,  40, 255 };
    static constexpr SDL_Color kColNoSel = { 110, 110, 110, 255 };
    static constexpr SDL_Color kColTbBg  = {  30,  30,  35, 255 };
    static constexpr SDL_Color kColTbBor = {  70,  70,  80, 255 };
    static constexpr SDL_Color kColTbFoc = {  70, 130, 180, 255 };

    IEGameObject* m_target     = nullptr;
    IETextBox*    m_focusedBox = nullptr;
    bool          m_prevLMB    = false;

    IEPanelLayout m_layout;
    IETextBox     m_tbX;
    IETextBox     m_tbY;
    IETextBox     m_tbZ;
};
