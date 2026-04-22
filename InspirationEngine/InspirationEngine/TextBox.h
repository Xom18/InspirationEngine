#pragma once

#define dTEXT_BOX_STYLE_EDITABLE		0x0001	//편집가능
#define dTEXT_BOX_STYLE_SELECTABLE		0x0002	//선택가능(드래그 포함)
#define dTEXT_BOX_STYLE_MULTILINE		0x0004	//여러줄 가능
#define dTEXT_BOX_STYLE_ENTER_TO_END	0x0008	//엔터 치면 종료(이경우엔 쉬프트 누르고 엔터 쳐야 개행)
#define dTEXT_BOX_AUTO_NEXTLINE			0x0010	//너비꽉차면 알아서 줄바꿔 출력
#define dTEXT_BOX_AUTO_SPACE_NEXTLINE	0x0030	//너비꽉차면 스페이스바 맞춰서 이쁘게 줄바꿔 출력

#define dTEXT_BOX_ALIGN_LEFT	0x01	//왼쪽
#define dTEXT_BOX_ALIGN_CENTER	0x02	//중앙
#define dTEXT_BOX_ALIGN_RIGHT	0x04	//오른쪽
#define dTEXT_BOX_ALIGN_TOP		0x10	//상단
#define dTEXT_BOX_ALIGN_MID		0x20	//중단
#define dTEXT_BOX_ALIGN_BOTTOM	0x40	//하단

class cTextTexture
{
public:
	SDL_Texture* m_pTexture = nullptr;	//텍스트 텍스쳐
	int m_iBufferPos = 0;			//이 텍스쳐의 텍스트가 버퍼의 어디부터인지
	SDL_Rect m_rtRect;			//텍스쳐 Rect

	cTextTexture()
	{
		memset(&m_rtRect, 0, sizeof(m_rtRect));
	}

	~cTextTexture()
	{
		if (m_pTexture)
			SDL_DestroyTexture(m_pTexture);
	}

	void reset()
	{
		memset(&m_rtRect, 0, sizeof(m_rtRect));
		if (m_pTexture)
			SDL_DestroyTexture(m_pTexture);
	}
};

//포커스 와있나 등 처리 필요할듯
class cTextBox : public cMenu
{
public:

private:
	cFont* m_lpFont = nullptr;		//그리는대 사용 할 폰트
	std::list<cTextTexture*> m_listTexture;//텍스트 텍스쳐들 있는 list
	std::string m_strText;			//원본 텍스트
	SDL_Color m_Color;				//컬러
	int m_iTextLeading = 0;			//행간
	int m_iFontHeight = 0;			//폰트 높이
	size_t m_szCursorPos = std::string::npos;//커서 위치
	int m_iSelectBegPos = 0;		//선택 시작영역
	int m_iSelectEndPos = 0;		//선택 종료영역
	int m_iTextBoxStyle = 0;		//텍스트 박스 스타일
	int m_iTextAlign = 0;			//텍스트 정렬
	SDL_Rect m_rtRect;
	bool m_bTextChanged = false;	//텍스트가 바뀜
	size_t m_szDrawHash = 0;		//이전에 그렸는지 확인을 위한 해시

	size_t m_szIMEInputLength = 0;	//한글이나 IME에서 한문자로 인식되서 일본어 같은거 처리용

	SDL_Point m_CursorScreenPos;	//화면상 커서 위치

	std::vector<size_t> m_vecGraphemeBounds;	//그래핌 클러스터 바이트 경계 목록 (lazy 재빌드)

public:
	cTextBox()
	{
		memset(&m_Color, 0xff, sizeof(m_Color));
		memset(&m_rtRect, 0, sizeof(m_rtRect));
	}

	~cTextBox()
	{
		resetTexture();
	}

	virtual void draw();
	virtual void update();

	/// <summary>
	/// 텍스쳐 초기화
	/// </summary>
	void resetTexture()
	{
		std::list<cTextTexture*>::iterator ite = m_listTexture.begin();
		for (; ite != m_listTexture.end(); ++ite)
		{
			cTextTexture* pTTexture = *ite;
			delete pTTexture;
		}
		m_listTexture.clear();
	}

	int getTextLength()
	{
		return static_cast<int>(m_strText.length());
	}

	void setFont(cFont* _lpFont)
	{
		m_lpFont = _lpFont;
#ifdef IE_LEGACY_TTF
		TTF_Font* lpTTF = m_lpFont->get();
		if (lpTTF != nullptr)
			m_iFontHeight = TTF_FontHeight(lpTTF);
#else
		m_iFontHeight = m_lpFont->getHeight();
#endif
	}

	void setText(const char* _csText)
	{
		m_strText = _csText;
		m_bTextChanged = true;
		m_vecGraphemeBounds.clear();
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
	size_t getCusorPos()
	{
		return m_szCursorPos;
	}

	/// <summary>
	/// 커서 위치 설정
	/// </summary>
	/// <param name="_iCusorPos">커서 위치(메모리 상에서)</param>
	void setCusorPos(size_t _szCusorPos)
	{
		m_szCursorPos = _szCusorPos;
	}

	void setCursurScreenPos(int x, int y)
	{
		m_CursorScreenPos.x = x;
		m_CursorScreenPos.y = y;
	}

	SDL_Point getCursurScreenPos()
	{
		return m_CursorScreenPos;
	}

	int getFontHeight()
	{
		return m_iFontHeight;
	}

	/// <summary>
	/// 커서 위치 설정
	/// </summary>
	/// <param name="_iCusorPos">커서 위치(문자 상에서)</param>
	/// <returns></returns>
	void setCusorCharPos(size_t _szCusorPos)
	{
		m_szCursorPos = cStrUTF8::getMemoryPoint(m_strText, _szCusorPos);
	}

	/// <summary>
	/// 커서 다음으로 이동
	/// </summary>
	void cusorMoveNext()
	{
		if (m_szCursorPos == std::string::npos)
		{
			m_szCursorPos = 0;
			return;
		}
		if (m_szCursorPos >= m_strText.length())
			return;

		if (m_vecGraphemeBounds.empty())
			rebuildGraphemeBounds();

		auto it = std::upper_bound(m_vecGraphemeBounds.begin(), m_vecGraphemeBounds.end(), m_szCursorPos);
		if (it != m_vecGraphemeBounds.end())
			m_szCursorPos = *it;
	}

	/// <summary>
	/// 커서 이전으로 이동
	/// </summary>
	void cusorMovePrevious()
	{
		if (m_szCursorPos == std::string::npos)
		{
			m_szCursorPos = 0;
			return;
		}
		if (m_szCursorPos == 0)
			return;

		if (m_vecGraphemeBounds.empty())
			rebuildGraphemeBounds();

		auto it = std::lower_bound(m_vecGraphemeBounds.begin(), m_vecGraphemeBounds.end(), m_szCursorPos);
		if (it != m_vecGraphemeBounds.begin())
		{
			--it;
			m_szCursorPos = *it;
		}
	}

	/// <summary>
	/// _iPoint로 부터 앞에있는 문자 _iCount개 삭제
	/// </summary>
	/// <param name="_iPoint">포인트(메모리상에서의 위치일수도 있고 문자에서의 위치일수도 있고)</param>
	/// <param name="_iCount">지울 개수</param>
	/// <param name="_bIsMemPoint">_iPoint가 메모리상에서의 위치인지</param>
	void removeByBackspace(size_t _szCount = 1)
	{
		//커서가 없다
		if (m_szCursorPos == std::string::npos)
			return;
		size_t szBefore = m_strText.length();
		cStrUTF8::removeToFront(m_strText, m_szCursorPos, _szCount);
		size_t szAfter = m_strText.length();

		m_szCursorPos -= szBefore - szAfter;
		m_bTextChanged = true;
		m_vecGraphemeBounds.clear();
	}

	/// <summary>
	/// _iPoint로 부터 뒤에있는 문자 _iCount개 삭제
	/// </summary>
	/// <param name="_iPoint">포인트(메모리상에서의 위치일수도 있고 문자에서의 위치일수도 있고)</param>
	/// <param name="_iCount">지울 개수</param>
	/// <param name="_bIsMemPoint">_iPoint가 메모리상에서의 위치인지</param>
	void removeByDelete(size_t _szCount = 1)
	{
		cStrUTF8::removeToBack(m_strText, m_szCursorPos, _szCount);
		m_bTextChanged = true;
		m_vecGraphemeBounds.clear();
	}

	void removeIMEInput()
	{
		//지울게 없는경우
		if (m_szIMEInputLength == 0)
			return;

		//비정상적인 경우
		if (m_szIMEInputLength > m_szCursorPos)
			return;

		//입력되있는 IME길이만큼 지움
		m_szCursorPos = m_szCursorPos - m_szIMEInputLength;
		m_strText.erase(m_szCursorPos, m_szIMEInputLength);
		m_szIMEInputLength = 0;
		m_bTextChanged = true;
		m_vecGraphemeBounds.clear();
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
		//커서가 없다
		if (m_szCursorPos == std::string::npos)
			return;
		m_strText.insert(m_szCursorPos, _csText);
		m_szCursorPos += std::strlen(_csText);
		m_bTextChanged = true;
		m_vecGraphemeBounds.clear();
	}

	void setPos(int _iX, int _iY)
	{
		m_rtRect.x = _iX;
		m_rtRect.y = _iY;
	}

	void setRect(int _iX, int _iY, int _iW = 0, int _iH = 0)
	{
		m_rtRect.x = _iX;
		m_rtRect.y = _iY;
		m_rtRect.w = _iW;
		m_rtRect.h = _iH;
	}

	void getRect(SDL_Rect& rect)
	{
		rect = m_rtRect;
	}

	void setStyle(int _iStyle)
	{
		m_iTextBoxStyle = _iStyle;
	}

	bool operateStyleCode(const std::string* _lpStrText, int* _lpOutResult);
	bool operateColorCode(const std::string* _lpStrText, SDL_Color* _lpOutResult);

	void setIMELength(size_t _szIMELength)
	{
		m_szIMEInputLength = _szIMELength;
	}

	void setDefaultColor(Uint8 _A, Uint8 _R, Uint8 _G, Uint8 _B)
	{
		m_Color.a = _A;
		m_Color.r = _R;
		m_Color.g = _G;
		m_Color.b = _B;
	}

	void setDefaultColor(SDL_Color _Color)
	{
		m_Color = _Color;
	}

private:
	void rebuildGraphemeBounds()
	{
		m_vecGraphemeBounds.clear();
		const auto* u = (const utf8proc_uint8_t*)m_strText.data();
		utf8proc_ssize_t len = (utf8proc_ssize_t)m_strText.size();

		m_vecGraphemeBounds.push_back(0);
		if (len == 0)
			return;

		utf8proc_int32_t cp1, cp2;
		utf8proc_int32_t state = 0;
		utf8proc_ssize_t i = 0;

		utf8proc_ssize_t n = utf8proc_iterate(u, len, &cp1);
		if (n <= 0) { m_vecGraphemeBounds.push_back((size_t)len); return; }
		i = n;

		while (i < len) {
			utf8proc_ssize_t m = utf8proc_iterate(u + i, len - i, &cp2);
			if (m <= 0) m = 1;
			if (utf8proc_grapheme_break_stateful(cp1, cp2, &state))
				m_vecGraphemeBounds.push_back((size_t)i);
			cp1 = cp2;
			i += m;
		}
		m_vecGraphemeBounds.push_back((size_t)len);
	}

};