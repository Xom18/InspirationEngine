#pragma once

class IERenderer;

class IEUIBase
{
public:
    IEUIBase() = default;

    /// <summary>
    /// 렌더러 설정
    /// </summary>
    /// <param name="renderer">사용할 렌더러</param>
    void SetRenderer(IERenderer* renderer)
    {
        m_renderer = renderer;
    }

    /// <summary>
    /// 그리기 (서브클래스에서 오버라이드)
    /// </summary>
    virtual void Draw() {}

    /// <summary>
    /// 업데이트 (서브클래스에서 오버라이드)
    /// </summary>
    virtual void Update() {}

protected:
    /// <summary>
    /// 렌더러 포인터 반환
    /// </summary>
    IERenderer* GetRenderer() { return m_renderer; }

private:
    IERenderer* m_renderer = nullptr;
};
