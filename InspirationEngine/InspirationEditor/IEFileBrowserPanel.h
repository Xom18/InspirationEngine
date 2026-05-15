#pragma once
#include "IEEditorPanel.h"

class IEAtlasEditorWindow;

class IEFileBrowserPanel : public IEEditorPanel
{
public:
    IEFileBrowserPanel();

    virtual void        Draw(IERenderer* r)                                override;
    virtual void        Update(float dt)                                   override;
    virtual const char* GetTitle()                               const override { return "Resources"; }
    virtual void        SetContentRect(int32_t x, int32_t y, int32_t w, int32_t h) override;
    virtual void        SetFont(IEFont* f)                                 override;
    virtual void        SetOwnerWindow(IEWindow* w)                        override;
    virtual void        SetRenderer(IERenderer* r)                         override;

    void SetAtlasEditor(IEAtlasEditorWindow* ae) { m_atlasEditor = ae; }

private:
    void OnSelect(const std::string& path);

    IEFileBrowser        m_fileBrowser;
    IEAtlasEditorWindow* m_atlasEditor = nullptr;
};
