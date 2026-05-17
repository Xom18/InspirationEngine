#pragma once
#include "IEEditorPanel.h"

/// <summary>
/// 카메라 파라미터 조정 패널. zoom / position 슬라이더.
/// </summary>
class IECameraPanel : public IEEditorPanel
{
public:
    /// <summary>
    /// 슬라이더 범위·콜백 및 레이아웃 초기화
    /// </summary>
    IECameraPanel();

    /// <summary>
    /// 카메라 슬라이더 및 정보를 렌더링
    /// </summary>
    /// <param name="r">사용할 렌더러</param>
    virtual void        Draw(IERenderer* r)                                         override;

    /// <summary>
    /// 카메라 상태와 슬라이더 동기화 및 입력 처리
    /// </summary>
    /// <param name="dt">경과 시간 (초)</param>
    virtual void        Update(float dt)                                            override;

    /// <summary>
    /// 패널 타이틀 문자열 반환
    /// </summary>
    virtual const char* GetTitle()                                       const override { return IELocalize::Get("panel.camera"); }

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
    virtual void        SetFont(IEFont* f)                                          override;

    /// <summary>
    /// 소유 창 설정 — 내부 위젯에 전파
    /// </summary>
    /// <param name="w">소유 창 포인터</param>
    virtual void        SetOwnerWindow(IEWindow* w)                                 override;

    /// <summary>
    /// 렌더러 설정 — 내부 위젯에 전파
    /// </summary>
    /// <param name="r">사용할 렌더러</param>
    virtual void        SetRenderer(IERenderer* r)                                  override;

    /// <summary>
    /// 편집 대상 카메라 오브젝트 설정 — 슬라이더 값 즉시 동기화.
    /// IECameraComponent + IETransformComponent 를 통해 편집.
    /// </summary>
    /// <param name="obj">IECameraComponent 를 보유한 IEGameObject (nullptr 가능)</param>
    void SetCameraObject(IEGameObject* obj);

private:
    static constexpr SDL_Color kColBg   = {  35,  35,  40, 255 };
    static constexpr SDL_Color kColText = { 180, 180, 180, 255 };

    IEGameObject* m_cameraObj = nullptr;
    IEPanelLayout m_layout;

    IEUISlider m_sliderZoom;
    IEUISlider m_sliderX;
    IEUISlider m_sliderY;
};
