#pragma once
#include "IEEditorPanel.h"

/// <summary>
/// 씬의 게임 카메라로 렌더링한 읽기 전용 미리보기 패널.
/// Viewport 의 에디터 카메라와 독립적으로 동작.
/// </summary>
class IECameraViewPanel : public IEEditorPanel
{
public:
    /// <summary>
    /// 미리보기할 씬 설정
    /// </summary>
    /// <param name="scene">대상 씬 포인터</param>
    void SetScene(IEScene* scene) { m_scene = scene; }

    /// <summary>
    /// 씬을 게임 카메라로 렌더링
    /// </summary>
    /// <param name="r">사용할 렌더러</param>
    virtual void        Draw(IERenderer* r)                                          override;

    /// <summary>
    /// 패널 입력 없음 — 읽기 전용
    /// </summary>
    virtual void        Update(float /*dt*/)                                         override {}

    /// <summary>
    /// 패널 타이틀 문자열 반환
    /// </summary>
    virtual const char* GetTitle()                                        const override { return IELocalize::Get("panel.cameraview"); }

    /// <summary>
    /// 패널 콘텐츠 영역 설정
    /// </summary>
    /// <param name="x">좌상단 X</param>
    /// <param name="y">좌상단 Y</param>
    /// <param name="w">너비</param>
    /// <param name="h">높이</param>
    virtual void        SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h) override;

private:
    static constexpr SDL_Color kColBg   = {  25,  25,  28, 255 };
    static constexpr SDL_Color kColText = { 100, 100, 100, 255 };

    IEScene* m_scene = nullptr;
    int32_t  m_x = 0, m_y = 0, m_w = 1, m_h = 1;
};
