#pragma once

class cEditBox : public cMenu
{
private:
	std::string m_strText;	//입력되있는 텍스트
	SDL_Rect m_rtInputArea;	//입력 영역
	int m_iCursorPos;		//커서 위치
	int m_iSelectBegPos;	//선택 시작영역
	int m_iSelectEndPos;	//선택 종료영역
	bool m_bIsEditing;		//작성중인지
	TTF_Font* m_lpFont;		//그리는대 사용 할 폰트

public:


private:


public:


};