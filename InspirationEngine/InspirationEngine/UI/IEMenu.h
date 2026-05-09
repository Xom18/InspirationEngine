#pragma once

class IETextBox;
class IERenderer;

enum class MenuType : int32_t
{
	Editbox = 0,
};

class IEMenu
{
public:
	IEMenu()
	{
		memset(&m_rect, 0, sizeof(m_rect));
	}

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
	int32_t     m_type     = 0;
	bool        m_isHide   = false;
	SDL_Rect    m_rect;
};
