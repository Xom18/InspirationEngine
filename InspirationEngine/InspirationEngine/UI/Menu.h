#pragma once

class TextBox;
class IERenderer;

enum class MenuType : int32_t
{
	Editbox = 0,
};

class Menu
{
public:


private:

protected:
	IERenderer* m_renderer = nullptr;
	int32_t m_type = 0;		//유형
	bool m_isHide = false;	//숨겨져있나
	SDL_Rect m_rect;		//영역


public:
	Menu()
	{
		memset(&m_rect, 0, sizeof(m_rect));
	}

	/// <summary>
	/// 렌더러 설정
	/// </summary>
	/// <param name="renderer">사용할 렌더러</param>
	void setRenderer(IERenderer* renderer)
	{
		m_renderer = renderer;
	}

	/// <summary>
	/// 그리기 (서브클래스에서 오버라이드)
	/// </summary>
	virtual void draw() {}

	/// <summary>
	/// 업데이트 (서브클래스에서 오버라이드)
	/// </summary>
	virtual void update() {}

};
