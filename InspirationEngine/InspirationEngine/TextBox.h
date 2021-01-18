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

	/// <summary>
	///	커서 위치 받아오기
	/// </summary>
	int getCusorPos()
	{
		return m_iCursorPos;
	}

	/// <summary>
	/// 커서 위치 설정
	/// </summary>
	/// <param name="_iCusorPos">커서 위치(메모리 상에서)</param>
	bool setCusorPos(int _iCusorPos)
	{
		m_iCursorPos = m_iCursorPos;
	}

	/// <summary>
	/// 커서 위치 설정
	/// </summary>
	/// <param name="_iCusorPos">커서 위치(문자 상에서)</param>
	/// <returns></returns>
	void setCusorCharPos(int _iCusorPos)
	{
		m_iCursorPos = cStrUTF8::getMemoryPoint(&m_strText, _iCusorPos);
	}

	/// <summary>
	/// 커서 다음으로 이동
	/// </summary>
	void cusorMoveNext()
	{
		//유효하지가 않아
		if(m_iCursorPos == -1)
		{
			m_iCursorPos = 0;
			return;
		}
		
		//이미 끝이야
		if(m_iCursorPos >= m_strText.length())
			return;

		//아스키면 바로 다음으로
		const char* csText = m_strText.c_str();
		if(cStrUTF8::isCharType(csText[m_iCursorPos]) == dCHAR_TYPE_ASCII)
		{
			++m_iCursorPos;
			return;
		}

		//UTF-8이다
		int iNewPos = m_iCursorPos + 1;
		for(; iNewPos < m_strText.length(); ++iNewPos)
		{
			if(cStrUTF8::isCharType(csText[iNewPos]) != dCHAR_TYPE_UTF8_M)
				break;
		}
		m_iCursorPos = iNewPos;
	}

	/// <summary>
	/// 커서 이전으로 이동
	/// </summary>
	void cusorMovePrevious()
	{
		//0보다 작을 수 없다
		if(m_iCursorPos <= 0)
		{
			m_iCursorPos = 0;
			return;
		}

		//이 앞에가 아스키면 바로 다음으로
		const char* csText = m_strText.c_str();
		if(cStrUTF8::isCharType(csText[m_iCursorPos - 1]) == dCHAR_TYPE_ASCII)
		{
			--m_iCursorPos;
			return;
		}

		//UTF-8이다
		int iNewPos = m_iCursorPos - 1;
		for(; iNewPos >= 0; --iNewPos)
		{
			if(cStrUTF8::isCharType(csText[iNewPos]) == dCHAR_TYPE_UTF8_B)
				break;
		}
		m_iCursorPos = iNewPos;
	}

	/// <summary>
	/// _iPoint로 부터 앞에있는 문자 _iCount개 삭제
	/// </summary>
	/// <param name="_iPoint">포인트(메모리상에서의 위치일수도 있고 문자에서의 위치일수도 있고)</param>
	/// <param name="_iCount">지울 개수</param>
	/// <param name="_bIsMemPoint">_iPoint가 메모리상에서의 위치인지</param>
	void removeByBackspace(int _iCount = 1)
	{
		if(m_iCursorPos <= 0)
			return;
		int iBefore = static_cast<int>(m_strText.length());
		cStrUTF8::removeToFront(&m_strText, m_iCursorPos, _iCount);
		int iAfter = static_cast<int>(m_strText.length());

		m_iCursorPos -= iBefore - iAfter;
		m_bTextChanged = true;
	}

	/// <summary>
	/// _iPoint로 부터 뒤에있는 문자 _iCount개 삭제
	/// </summary>
	/// <param name="_iPoint">포인트(메모리상에서의 위치일수도 있고 문자에서의 위치일수도 있고)</param>
	/// <param name="_iCount">지울 개수</param>
	/// <param name="_bIsMemPoint">_iPoint가 메모리상에서의 위치인지</param>
	void removeByDelete(int _iCount = 1)
	{
		cStrUTF8::removeToBack(&m_strText, m_iCursorPos, _iCount);
		m_bTextChanged = true;
	}

	/// <summary>
	/// 문자 추가
	/// </summary>
	/// <param name="_csText">문자 위치</param>
	/// <param name="_iInsertPoint">문자 넣을 위치</param>
	void insert(const char* _csText, int _iInsertPoint)
	{

	}

	void insertCusorPos(const char* _csText)
	{
		int iCount = static_cast<int>(std::strlen(_csText));
		m_strText.insert(m_iCursorPos, _csText);
		m_iCursorPos += iCount;
		m_bTextChanged = true;
	}

private:

};