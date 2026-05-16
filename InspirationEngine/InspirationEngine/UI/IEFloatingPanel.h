#pragma once
#include "../Window/IEWindow.h"
#include "IEPanel.h"
#include <memory>
#include <string>

/// <summary>
/// 패널이 창 밖으로 드래그됐을 때 생성되는 독립 SDL 창.
/// CallXButton() 호출 시 IECore::RequestRemoveWindow 로 안전하게 소멸.
/// </summary>
class IEFloatingPanel : public IEWindow
{
public:
    /// <summary>
    /// 패널 소유권 이전 및 위젯 초기화
    /// </summary>
    /// <param name="panel">소유권을 이전받을 패널</param>
    /// <param name="font">사용할 폰트</param>
    /// <param name="windowId">IECore 등록에 사용할 고유 창 ID</param>
    void Init(std::unique_ptr<IEPanel> panel, IEFont* font, const std::string& windowId);

    /// <summary>
    /// 창 콘텐츠 렌더링
    /// </summary>
    virtual void Draw()                          override;

    /// <summary>
    /// 패널 입력 처리 및 상태 갱신
    /// </summary>
    /// <param name="dt">경과 시간 (초)</param>
    virtual void Update(float dt)                override;

    /// <summary>
    /// 닫기 버튼 동작 — IECore::RequestRemoveWindow 로 창 제거
    /// </summary>
    virtual void CallXButton()                   override;

    /// <summary>
    /// 창 크기 변경 시 패널 콘텐츠 영역 재계산
    /// </summary>
    /// <param name="w">새 창 너비</param>
    /// <param name="h">새 창 높이</param>
    virtual void OnResize(int32_t w, int32_t h)  override;

    /// <summary>
    /// 이 창의 IECore 등록 ID 반환
    /// </summary>
    const std::string& GetWindowId() const { return m_windowId; }

private:
    static constexpr int32_t   kDragBarH   = 24;
    static constexpr int32_t   kCloseW     = 24;
    static constexpr SDL_Color kColDragBar  = {  55,  55,  62, 255 };
    static constexpr SDL_Color kColDragText = { 200, 200, 200, 255 };
    static constexpr SDL_Color kColClose    = { 160,  50,  50, 255 };

    std::unique_ptr<IEPanel> m_panel;
    IEFont*     m_font        = nullptr;
    std::string m_windowId;

    bool    m_dragging    = false;
    int32_t m_dragOffsetX = 0;
    int32_t m_dragOffsetY = 0;
    bool    m_prevLMB     = false;
};
