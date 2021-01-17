#pragma once

class cTextBox;
class cRenderer;

enum
{
	eMENU_TYPE_EDITBOX,
};

class cMenu
{
public:


private:

protected:
	cRenderer* m_lpRenderer = nullptr;
	int m_iType = 0;		//유형
	bool m_bIsHide = false;	//숨겨져있나
	SDL_Rect m_rtRect;		//영역


public:
	cMenu()
	{
		memset(&m_rtRect, 0, sizeof(m_rtRect));
	}

	void setRenderer(cRenderer* _lpRenderer)
	{
		m_lpRenderer = _lpRenderer;
	}
	virtual void draw(){}
	virtual void update(){}

};