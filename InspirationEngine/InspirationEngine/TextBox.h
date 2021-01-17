#pragma once

#define dTEXT_BOX_STYLE_EDITABLE		0x0001	//편집가능
#define dTEXT_BOX_STYLE_SELECTABLE		0x0002	//선택가능(드래그 포함)
#define dTEXT_BOX_STYLE_MULTILINE		0x0004	//여러줄 가능
#define dTEXT_BOX_STYLE_ENTER_TO_END	0x0008	//엔터 치면 종료(쉬프트 누르고 엔터 처야 개행)

#define dTEXT_BOX_ALIGN_LEFT	0x01	//왼쪽
#define dTEXT_BOX_ALIGN_CENTER	0x02	//중앙
#define dTEXT_BOX_ALIGN_RIGHT	0x04	//오른쪽
#define dTEXT_BOX_ALIGN_TOP		0x10	//상단
#define dTEXT_BOX_ALIGN_MID		0x20	//중단
#define dTEXT_BOX_ALIGN_BOTTOM	0x40	//하단

class cTextBox : public cMenu
{
public:

private:
	TTF_Font* m_lpFont = nullptr;		//그리는대 사용 할 폰트
	SDL_Texture* m_pTexture = nullptr;//텍스트 텍스쳐
	std::string m_strText;		//텍스트
	SDL_Color m_Color;			//컬러
	int m_iCursorPos = 0;		//커서 위치
	int m_iSelectBegPos = 0;	//선택 시작영역
	int m_iSelectEndPos = 0;	//선택 종료영역
	int m_iTextBoxStyle = 0;	//텍스트 박스 스타일
	int m_iTextAlign = 0;		//텍스트 정렬
	bool m_bTextChanged = false;	//텍스트가 바뀜
	size_t m_szDrawHash = 0;	//이전에 그렸는지 확인을 위한 해시

public:
	cTextBox()
	{
		memset(&m_Color, 0xff, sizeof(m_Color));
	}

	~cTextBox()
	{
		//텍스쳐 해제
		if(m_pTexture)
			SDL_DestroyTexture(m_pTexture);
	}

	virtual void draw();
	virtual void update();

	int getTextLength()
	{
		return static_cast<int>(m_strText.length());
	}

	void setFont(TTF_Font* _lpFont)
	{
		m_lpFont = _lpFont;
	}

	void setText(const char* _csText)
	{
		m_strText = _csText;
		m_bTextChanged = true;
	}

	//텍스트 텍스쳐화
	void transToTexture();

	const char* getText()
	{
		return m_strText.c_str();
	}

	//맨 마지막 텍스트 지우기
	void popBack()
	{
		if(m_strText.length() == 0)
			return;

		const char* lpText = m_strText.c_str();
		int iLength = static_cast<int>(m_strText.length());

		if((lpText[iLength - 1] & 0b10000000) == 0)
		{//아스키쪽이면 단순히 마지막 제거
			m_strText.pop_back();
		}
		else
		{//UTF-8이다
			//몇개 지워야되는지 체크
			int iDeleteCount = 0;
			for(int i = iLength - 1; i >= 0; --i)
			{
				++iDeleteCount;
				if((lpText[i] & 0b11000000) == 0b11000000)
					break;
			}

			//지워야되는 만큼 삭제
			m_strText.erase(static_cast<size_t>(iLength) - iDeleteCount);
		}
		m_bTextChanged = true;
	}

	//맨 마지막에 텍스트 추가
	void pushBack(const char* _csText)
	{
		m_strText += _csText;
		m_bTextChanged = true;
	}

private:

};