#pragma once
#include "IEEditorPanel.h"

class IEAtlasEditorWindow;

class IEFileBrowserPanel : public IEEditorPanel
{
public:
    /// <summary>
    /// 파일 브라우저 위젯 및 경로 초기화
    /// </summary>
    IEFileBrowserPanel();

    /// <summary>
    /// 파일 트리를 렌더링
    /// </summary>
    /// <param name="r">사용할 렌더러</param>
    virtual void        Draw(IERenderer* r)                                override;

    /// <summary>
    /// 파일 트리 입력 처리 및 상태 갱신
    /// </summary>
    /// <param name="dt">경과 시간 (초)</param>
    virtual void        Update(float dt)                                   override;

    /// <summary>
    /// 패널 타이틀 문자열 반환
    /// </summary>
    virtual const char* GetTitle()                               const override { return IELocalize::Get("panel.resources"); }

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
    virtual void        SetFont(IEFont* f)                                 override;

    /// <summary>
    /// 소유 창 설정 — 내부 위젯에 전파
    /// </summary>
    /// <param name="w">소유 창 포인터</param>
    virtual void        SetOwnerWindow(IEWindow* w)                        override;

    /// <summary>
    /// 렌더러 설정 — 내부 위젯에 전파
    /// </summary>
    /// <param name="r">사용할 렌더러</param>
    virtual void        SetRenderer(IERenderer* r)                         override;

    /// <summary>
    /// 파일 선택 시 아틀라스 에디터로 전달할 참조 설정
    /// </summary>
    /// <param name="ae">아틀라스 에디터 창 포인터</param>
    void SetAtlasEditor(IEAtlasEditorWindow* ae) { m_atlasEditor = ae; }

private:
    void OnSelect(const std::string& path);

    IEFileBrowser        m_fileBrowser;
    IEAtlasEditorWindow* m_atlasEditor = nullptr;
};
