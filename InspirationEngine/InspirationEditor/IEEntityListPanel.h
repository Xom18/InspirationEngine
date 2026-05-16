#pragma once
#include "IEEditorPanel.h"

/// <summary>
/// 씬 오브젝트 목록 패널. 클릭 시 선택 연동 예정.
/// </summary>
class IEEntityListPanel : public IEEditorPanel
{
public:
    virtual void        Draw(IERenderer* r)                                override;
    virtual void        Update(float dt)                                   override;
    virtual const char* GetTitle()                               const override { return "Entities"; }
    virtual void        SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h) override;
    void SetScene(IEScene* scene) { m_scene = scene; }

private:
    static constexpr int32_t  kRowH      = 22;
    static constexpr SDL_Color kColBg    = {  35,  35,  40, 255 };
    static constexpr SDL_Color kColRow   = {  45,  45,  52, 255 };
    static constexpr SDL_Color kColSelRow = { 60,  90, 140, 255 };
    static constexpr SDL_Color kColText  = { 200, 200, 200, 255 };

    IEScene* m_scene = nullptr;

    int32_t m_x = 0, m_y = 0, m_w = 0, m_h = 0;
    int32_t m_selectedIndex = -1;
    bool    m_prevLMB       = false;
};
