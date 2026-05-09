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

class TextTexture
{
	friend class TextBox;

public:
	TextTexture() = default;

	~TextTexture()
	{
		if (m_texture != nullptr)
			SDL_DestroyTexture(m_texture);
	}

	void reset()
	{
		m_rect = {};
		if (m_texture != nullptr)
			SDL_DestroyTexture(m_texture);
	}

private:
	SDL_Texture* m_texture   = nullptr;
	int32_t      m_bufferPos = 0;
	SDL_Rect     m_rect      = {};
};

//포커스 와있나 등 처리 필요할듯
class TextBox : public Menu
{
public:

private:
	Font* m_font = nullptr;				//그리는대 사용 할 폰트
	std::list<std::unique_ptr<TextTexture>> m_textures;	//텍스트 텍스쳐들 있는 list
	std::string m_text;					//원본 텍스트
	SDL_Color m_color;					//컬러
	int32_t m_textLeading = 0;			//행간
	int32_t m_fontHeight = 0;			//폰트 높이
	size_t m_cursorPos = std::string::npos;//커서 위치
	int32_t m_selectBegPos = 0;			//선택 시작영역
	int32_t m_selectEndPos = 0;			//선택 종료영역
	int32_t m_textBoxStyle = 0;			//텍스트 박스 스타일
	int32_t m_textAlign = 0;			//텍스트 정렬
	SDL_Rect m_rect;
	bool m_textChanged = false;			//텍스트가 바뀜
	size_t m_drawHash = 0;				//이전에 그렸는지 확인을 위한 해시

	size_t m_imeInputLength = 0;		//한글이나 IME에서 한문자로 인식되서 일본어 같은거 처리용

	SDL_Point m_cursorScreenPos;		//화면상 커서 위치

	std::vector<size_t> m_graphemeBounds;	//그래핌 클러스터 바이트 경계 목록 (lazy 재빌드)

public:
	TextBox()
	{
		memset(&m_color, 0xff, sizeof(m_color));
		memset(&m_rect, 0, sizeof(m_rect));
	}

	~TextBox()
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
		m_textures.clear();
	}

	/// <summary>
	/// 텍스트 바이트 길이 반환
	/// </summary>
	int32_t getTextLength()
	{
		return static_cast<int32_t>(m_text.length());
	}

	/// <summary>
	/// 폰트 설정
	/// </summary>
	/// <param name="font">사용할 폰트</param>
	void setFont(Font* font)
	{
		m_font = font;
		m_fontHeight = m_font->getHeight();
	}

	/// <summary>
	/// 텍스트 설정
	/// </summary>
	/// <param name="text">설정할 텍스트 (UTF-8)</param>
	void setText(const char* text)
	{
		m_text = text;
		m_textChanged = true;
		m_graphemeBounds.clear();
	}

	/// <summary>
	/// 텍스트를 텍스쳐로 변환
	/// </summary>
	void transToTexture();

	/// <summary>
	/// 현재 텍스트 반환 (UTF-8)
	/// </summary>
	const char* getText()
	{
		return m_text.c_str();
	}

	/// <summary>
	///	커서 위치 받아오기
	/// </summary>
	size_t getCusorPos()
	{
		return m_cursorPos;
	}

	/// <summary>
	/// 커서 위치 설정
	/// </summary>
	/// <param name="cursorPos">커서 위치(메모리 상에서)</param>
	void setCusorPos(size_t cursorPos)
	{
		m_cursorPos = cursorPos;
	}

	/// <summary>
	/// 화면상 커서 위치 설정
	/// </summary>
	/// <param name="x">화면 X</param>
	/// <param name="y">화면 Y</param>
	void setCursurScreenPos(int32_t x, int32_t y)
	{
		m_cursorScreenPos.x = x;
		m_cursorScreenPos.y = y;
	}

	/// <summary>
	/// 화면상 커서 위치 반환
	/// </summary>
	SDL_Point getCursurScreenPos()
	{
		return m_cursorScreenPos;
	}

	/// <summary>
	/// 폰트 높이 반환 (px)
	/// </summary>
	int32_t getFontHeight()
	{
		return m_fontHeight;
	}

	/// <summary>
	/// 커서 위치 설정 (문자 인덱스 기준)
	/// </summary>
	/// <param name="cursorPos">커서 위치 (UTF-8 문자 인덱스)</param>
	void setCusorCharPos(size_t cursorPos)
	{
		m_cursorPos = StrUTF8::getMemoryPoint(m_text, cursorPos);
	}

	/// <summary>
	/// 커서 다음으로 이동
	/// </summary>
	void cusorMoveNext()
	{
		if (m_cursorPos == std::string::npos)
		{
			m_cursorPos = 0;
			return;
		}
		if (m_cursorPos >= m_text.length())
			return;

		if (m_graphemeBounds.empty())
			rebuildGraphemeBounds();

		auto it = std::upper_bound(m_graphemeBounds.begin(), m_graphemeBounds.end(), m_cursorPos);
		if (it != m_graphemeBounds.end())
			m_cursorPos = *it;
	}

	/// <summary>
	/// 커서 이전으로 이동
	/// </summary>
	void cusorMovePrevious()
	{
		if (m_cursorPos == std::string::npos)
		{
			m_cursorPos = 0;
			return;
		}
		if (m_cursorPos == 0)
			return;

		if (m_graphemeBounds.empty())
			rebuildGraphemeBounds();

		auto it = std::lower_bound(m_graphemeBounds.begin(), m_graphemeBounds.end(), m_cursorPos);
		if (it != m_graphemeBounds.begin())
		{
			--it;
			m_cursorPos = *it;
		}
	}

	/// <summary>
	/// 커서 앞 문자 count개 삭제 (Backspace)
	/// </summary>
	/// <param name="count">삭제할 문자 수</param>
	void removeByBackspace(size_t count = 1)
	{
		//커서가 없다
		if (m_cursorPos == std::string::npos)
			return;
		size_t szBefore = m_text.length();
		StrUTF8::removeToFront(m_text, m_cursorPos, count);
		size_t szAfter = m_text.length();

		m_cursorPos -= szBefore - szAfter;
		m_textChanged = true;
		m_graphemeBounds.clear();
	}

	/// <summary>
	/// 커서 뒤 문자 count개 삭제 (Delete)
	/// </summary>
	/// <param name="count">삭제할 문자 수</param>
	void removeByDelete(size_t count = 1)
	{
		StrUTF8::removeToBack(m_text, m_cursorPos, count);
		m_textChanged = true;
		m_graphemeBounds.clear();
	}

	/// <summary>
	/// 조합 중인 IME 입력 제거
	/// </summary>
	void removeIMEInput()
	{
		//지울게 없는경우
		if (m_imeInputLength == 0)
			return;

		//비정상적인 경우
		if (m_imeInputLength > m_cursorPos)
			return;

		//입력되있는 IME길이만큼 지움
		m_cursorPos = m_cursorPos - m_imeInputLength;
		m_text.erase(m_cursorPos, m_imeInputLength);
		m_imeInputLength = 0;
		m_textChanged = true;
		m_graphemeBounds.clear();
	}

	/// <summary>
	/// 지정 위치에 문자 삽입
	/// </summary>
	/// <param name="text">삽입할 텍스트 (UTF-8)</param>
	/// <param name="insertPoint">삽입 위치 (바이트 오프셋)</param>
	void insert(const char* text, int32_t insertPoint)
	{

	}

	/// <summary>
	/// 현재 커서 위치에 문자 삽입
	/// </summary>
	/// <param name="text">삽입할 텍스트 (UTF-8)</param>
	void insertCusorPos(const char* text)
	{
		//커서가 없다
		if (m_cursorPos == std::string::npos)
			return;
		m_text.insert(m_cursorPos, text);
		m_cursorPos += std::strlen(text);
		m_textChanged = true;
		m_graphemeBounds.clear();
	}

	/// <summary>
	/// 텍스트 박스 위치 설정
	/// </summary>
	/// <param name="x">화면 X</param>
	/// <param name="y">화면 Y</param>
	void setPos(int32_t x, int32_t y)
	{
		m_rect.x = x;
		m_rect.y = y;
	}

	/// <summary>
	/// 텍스트 박스 영역 설정
	/// </summary>
	/// <param name="x">화면 X</param>
	/// <param name="y">화면 Y</param>
	/// <param name="w">너비 (0이면 제한 없음)</param>
	/// <param name="h">높이 (0이면 제한 없음)</param>
	void setRect(int32_t x, int32_t y, int32_t w = 0, int32_t h = 0)
	{
		m_rect.x = x;
		m_rect.y = y;
		m_rect.w = w;
		m_rect.h = h;
	}

	/// <summary>
	/// 텍스트 박스 영역 반환
	/// </summary>
	/// <param name="rect">결과를 받을 SDL_Rect</param>
	void getRect(SDL_Rect& rect)
	{
		rect = m_rect;
	}

	/// <summary>
	/// 텍스트 박스 스타일 설정
	/// </summary>
	/// <param name="style">dTEXT_BOX_* 플래그 조합</param>
	void setStyle(int32_t style)
	{
		m_textBoxStyle = style;
	}

	/// <summary>
	/// 스타일 마크업 코드 파싱
	/// </summary>
	/// <param name="strText">파싱할 마크업 문자열</param>
	/// <param name="outResult">파싱된 IE_FONT_STYLE_* 결과</param>
	bool operateStyleCode(const std::string* strText, int32_t* outResult);

	/// <summary>
	/// 색상 마크업 코드 파싱
	/// </summary>
	/// <param name="strText">파싱할 마크업 문자열</param>
	/// <param name="outResult">파싱된 SDL_Color 결과</param>
	bool operateColorCode(const std::string* strText, SDL_Color* outResult);

	/// <summary>
	/// 조합 중인 IME 입력 바이트 길이 설정
	/// </summary>
	/// <param name="imeLength">IME 입력 바이트 수</param>
	void setIMELength(size_t imeLength)
	{
		m_imeInputLength = imeLength;
	}

	/// <summary>
	/// 기본 텍스트 색상 설정
	/// </summary>
	/// <param name="_A">알파</param>
	/// <param name="_R">빨강</param>
	/// <param name="_G">초록</param>
	/// <param name="_B">파랑</param>
	void setDefaultColor(Uint8 _A, Uint8 _R, Uint8 _G, Uint8 _B)
	{
		m_color.a = _A;
		m_color.r = _R;
		m_color.g = _G;
		m_color.b = _B;
	}

	/// <summary>
	/// 기본 텍스트 색상 설정
	/// </summary>
	/// <param name="color">SDL_Color 색상</param>
	void setDefaultColor(SDL_Color color)
	{
		m_color = color;
	}

private:
	/// <summary>
	/// 그래핌 클러스터 바이트 경계 목록 재빌드
	/// </summary>
	void rebuildGraphemeBounds()
	{
		m_graphemeBounds.clear();
		const auto* u = reinterpret_cast<const utf8proc_uint8_t*>(m_text.data());
		utf8proc_ssize_t len = static_cast<utf8proc_ssize_t>(m_text.size());

		m_graphemeBounds.push_back(0);
		if (len == 0)
			return;

		utf8proc_int32_t cp1, cp2;
		utf8proc_int32_t state = 0;
		utf8proc_ssize_t i = 0;

		utf8proc_ssize_t n = utf8proc_iterate(u, len, &cp1);
		if (n <= 0) { m_graphemeBounds.push_back(static_cast<size_t>(len)); return; }
		i = n;

		while (i < len) {
			utf8proc_ssize_t m = utf8proc_iterate(u + i, len - i, &cp2);
			if (m <= 0) m = 1;
			if (utf8proc_grapheme_break_stateful(cp1, cp2, &state))
				m_graphemeBounds.push_back(static_cast<size_t>(i));
			cp1 = cp2;
			i += m;
		}
		m_graphemeBounds.push_back(static_cast<size_t>(len));
	}

};
