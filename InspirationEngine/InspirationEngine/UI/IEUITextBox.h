#pragma once

#define dTEXT_BOX_STYLE_EDITABLE		0x0001
#define dTEXT_BOX_STYLE_SELECTABLE		0x0002
#define dTEXT_BOX_STYLE_MULTILINE		0x0004
#define dTEXT_BOX_STYLE_ENTER_TO_END	0x0008
#define dTEXT_BOX_AUTO_NEXTLINE			0x0010
#define dTEXT_BOX_AUTO_SPACE_NEXTLINE	0x0030

#define dTEXT_BOX_ALIGN_LEFT	0x01
#define dTEXT_BOX_ALIGN_CENTER	0x02
#define dTEXT_BOX_ALIGN_RIGHT	0x04
#define dTEXT_BOX_ALIGN_TOP		0x10
#define dTEXT_BOX_ALIGN_MID		0x20
#define dTEXT_BOX_ALIGN_BOTTOM	0x40

class IETextTexture
{
	friend class IEUITextBox;

public:
	IETextTexture() = default;

	~IETextTexture()
	{
		if (m_texture != nullptr)
			SDL_DestroyTexture(m_texture);
	}

	/// <summary>
	/// 텍스쳐 및 영역 초기화
	/// </summary>
	void Reset()
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

class IEUITextBox : public IEUIBase
{
public:
	IEUITextBox()
	{
		memset(&m_color, 0xff, sizeof(m_color));
		memset(&m_rect, 0, sizeof(m_rect));
	}

	~IEUITextBox()
	{
		ResetTexture();
	}

	virtual void Draw() override;
	virtual void Update() override;

	/// <summary>
	/// 텍스쳐 초기화
	/// </summary>
	void ResetTexture()
	{
		m_textures.clear();
	}

	/// <summary>
	/// 텍스트 바이트 길이 반환
	/// </summary>
	int32_t GetTextLength()
	{
		return static_cast<int32_t>(m_text.length());
	}

	/// <summary>
	/// 폰트 설정
	/// </summary>
	/// <param name="font">사용할 폰트</param>
	virtual void SetFont(IEFont* font) override
	{
		IEUIBase::SetFont(font);
		m_font = font;
		if (m_font != nullptr)
			m_fontHeight = m_font->GetHeight();
	}

	/// <summary>
	/// 텍스트 설정
	/// </summary>
	/// <param name="text">설정할 텍스트 (UTF-8)</param>
	void SetText(const char* text)
	{
		m_text = text;
		m_textChanged = true;
		m_graphemeBounds.clear();
	}

	/// <summary>
	/// 텍스트를 텍스쳐로 변환
	/// </summary>
	void TransToTexture();

	/// <summary>
	/// 현재 텍스트 반환 (UTF-8)
	/// </summary>
	const char* GetText()
	{
		return m_text.c_str();
	}

	/// <summary>
	/// 커서 위치 받아오기
	/// </summary>
	size_t GetCursorPos()
	{
		return m_cursorPos;
	}

	/// <summary>
	/// 커서 위치 설정
	/// </summary>
	/// <param name="cursorPos">커서 위치 (바이트 오프셋)</param>
	void SetCursorPos(size_t cursorPos)
	{
		m_cursorPos = cursorPos;
	}

	/// <summary>
	/// 화면상 커서 위치 설정
	/// </summary>
	/// <param name="x">화면 X</param>
	/// <param name="y">화면 Y</param>
	void SetCursorScreenPos(int32_t x, int32_t y)
	{
		m_cursorScreenPos.x = x;
		m_cursorScreenPos.y = y;
	}

	/// <summary>
	/// 화면상 커서 위치 반환
	/// </summary>
	SDL_Point GetCursorScreenPos()
	{
		return m_cursorScreenPos;
	}

	/// <summary>
	/// 폰트 높이 반환 (px)
	/// </summary>
	int32_t GetFontHeight()
	{
		return m_fontHeight;
	}

	/// <summary>
	/// 커서 위치 설정 (문자 인덱스 기준)
	/// </summary>
	/// <param name="cursorPos">커서 위치 (UTF-8 문자 인덱스)</param>
	void SetCursorCharPos(size_t cursorPos)
	{
		m_cursorPos = IEStrUTF8::GetMemoryPoint(m_text, cursorPos);
	}

	bool IsMultiline() const { return (m_textBoxStyle & dTEXT_BOX_STYLE_MULTILINE) != 0; }

	/// <summary>
	/// 커서 다음으로 이동
	/// </summary>
	void CursorMoveNext()
	{
		if (m_cursorPos == std::string::npos)
		{
			m_cursorPos = 0;
			m_textChanged = true;
			return;
		}
		if (m_cursorPos >= m_text.length())
			return;

		if (m_graphemeBounds.empty())
			RebuildGraphemeBounds();

		auto it = std::upper_bound(m_graphemeBounds.begin(), m_graphemeBounds.end(), m_cursorPos);
		if (it != m_graphemeBounds.end())
		{
			m_cursorPos = *it;
			m_textChanged = true;
		}
	}

	/// <summary>
	/// 커서 이전으로 이동
	/// </summary>
	void CursorMovePrevious()
	{
		if (m_cursorPos == std::string::npos)
		{
			m_cursorPos = 0;
			m_textChanged = true;
			return;
		}
		if (m_cursorPos == 0)
			return;

		if (m_graphemeBounds.empty())
			RebuildGraphemeBounds();

		auto it = std::lower_bound(m_graphemeBounds.begin(), m_graphemeBounds.end(), m_cursorPos);
		if (it != m_graphemeBounds.begin())
		{
			--it;
			m_cursorPos = *it;
			m_textChanged = true;
		}
	}

	/// <summary>
	/// 커서 앞 문자 count개 삭제 (Backspace)
	/// </summary>
	/// <param name="count">삭제할 문자 수</param>
	void RemoveByBackspace(size_t count = 1)
	{
		if (m_cursorPos == std::string::npos)
			return;
		m_overwriteMode = false;
		size_t before = m_text.length();
		IEStrUTF8::RemoveToFront(m_text, m_cursorPos, count);
		size_t after = m_text.length();

		m_cursorPos -= before - after;
		m_textChanged = true;
		m_graphemeBounds.clear();
	}

	/// <summary>
	/// 커서 뒤 문자 count개 삭제 (Delete)
	/// </summary>
	/// <param name="count">삭제할 문자 수</param>
	void RemoveByDelete(size_t count = 1)
	{
		m_overwriteMode = false;
		IEStrUTF8::RemoveToBack(m_text, m_cursorPos, count);
		m_textChanged = true;
		m_graphemeBounds.clear();
	}

	/// <summary>
	/// 조합 중인 IME 입력 제거
	/// </summary>
	void RemoveIMEInput()
	{
		if (m_imeInputLength == 0)
			return;
		if (m_imeInputLength > m_cursorPos)
			return;

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
	void Insert(const char* /*text*/, int32_t /*insertPoint*/)
	{
	}

	/// <summary>
	/// 현재 커서 위치에 문자 삽입
	/// </summary>
	/// <param name="text">삽입할 텍스트 (UTF-8)</param>
	void InsertCursorPos(const char* text)
	{
		if (m_cursorPos == std::string::npos)
			return;
		if (m_overwriteMode)
		{
			m_text.clear();
			m_cursorPos    = 0;
			m_overwriteMode = false;
			m_graphemeBounds.clear();
		}
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
	void SetPos(int32_t x, int32_t y)
	{
		m_rect.x = x;
		m_rect.y = y;
		IEUIBase::SetRect(x, y, m_rect.w, m_rect.h);
	}

	/// <summary>
	/// 텍스트 박스 영역 설정
	/// </summary>
	/// <param name="x">화면 X</param>
	/// <param name="y">화면 Y</param>
	/// <param name="w">너비 (0이면 제한 없음)</param>
	/// <param name="h">높이 (0이면 제한 없음)</param>
	virtual void SetRect(int32_t x, int32_t y, int32_t w = 0, int32_t h = 0) override
	{
		IEUIBase::SetRect(x, y, w, h);
		m_rect.x = x;
		m_rect.y = y;
		m_rect.w = w;
		m_rect.h = h;
	}

	/// <summary>
	/// 텍스트 박스 영역 반환
	/// </summary>
	/// <param name="rect">결과를 받을 SDL_Rect</param>
	void GetRect(SDL_Rect& rect)
	{
		rect = m_rect;
	}

	/// <summary>
	/// 텍스트 박스 스타일 설정
	/// </summary>
	/// <param name="style">dTEXT_BOX_* 플래그 조합</param>
	void SetStyle(int32_t style)
	{
		m_textBoxStyle = style;
	}

	/// <summary>
	/// 스타일 마크업 코드 파싱
	/// </summary>
	/// <param name="strText">파싱할 마크업 문자열</param>
	/// <param name="outResult">파싱된 IE_FONT_STYLE_* 결과</param>
	bool OperateStyleCode(const std::string* strText, int32_t* outResult);

	/// <summary>
	/// 색상 마크업 코드 파싱
	/// </summary>
	/// <param name="strText">파싱할 마크업 문자열</param>
	/// <param name="outResult">파싱된 SDL_Color 결과</param>
	bool OperateColorCode(const std::string* strText, SDL_Color* outResult);

	/// <summary>
	/// 조합 중인 IME 입력 바이트 길이 설정
	/// </summary>
	/// <param name="imeLength">IME 입력 바이트 수</param>
	void SetIMELength(size_t imeLength)
	{
		m_imeInputLength = imeLength;
	}

	/// <summary>
	/// 기본 텍스트 색상 설정
	/// </summary>
	/// <param name="a">알파</param>
	/// <param name="r">빨강</param>
	/// <param name="g">초록</param>
	/// <param name="b">파랑</param>
	void SetDefaultColor(Uint8 a, Uint8 r, Uint8 g, Uint8 b)
	{
		m_color.a = a;
		m_color.r = r;
		m_color.g = g;
		m_color.b = b;
	}

	/// <summary>
	/// 기본 텍스트 색상 설정
	/// </summary>
	/// <param name="color">SDL_Color 색상</param>
	void SetDefaultColor(SDL_Color color)
	{
		m_color = color;
	}

private:
	/// <summary>
	/// 그래핌 클러스터 바이트 경계 목록 재빌드
	/// </summary>
	void RebuildGraphemeBounds()
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
		if (n <= 0)
		{
			m_graphemeBounds.push_back(static_cast<size_t>(len));
			return;
		}
		i = n;

		while (i < len)
		{
			utf8proc_ssize_t m = utf8proc_iterate(u + i, len - i, &cp2);
			if (m <= 0)
				m = 1;
			if (utf8proc_grapheme_break_stateful(cp1, cp2, &state))
				m_graphemeBounds.push_back(static_cast<size_t>(i));
			cp1 = cp2;
			i += m;
		}
		m_graphemeBounds.push_back(static_cast<size_t>(len));
	}

private:
	IEFont*                                   m_font          = nullptr;
	std::list<std::unique_ptr<IETextTexture>> m_textures;
	std::string                               m_text;
	SDL_Color                                 m_color;
	int32_t                                   m_textLeading   = 0;
	int32_t                                   m_fontHeight    = 0;
	size_t                                    m_cursorPos     = std::string::npos;
	int32_t                                   m_selectBegPos  = 0;
	int32_t                                   m_selectEndPos  = 0;
	int32_t                                   m_textBoxStyle  = 0;
	int32_t                                   m_textAlign     = 0;
	SDL_Rect                                  m_rect;
	bool                                      m_textChanged   = false;
	bool                                      m_prevLMB       = false;
	bool                                      m_overwriteMode = false;
	size_t                                    m_drawHash      = 0;
	size_t                                    m_imeInputLength = 0;
	SDL_Point                                 m_cursorScreenPos;
	std::vector<size_t>                       m_graphemeBounds;
	IERenderer*                               m_lastUsedIERenderer = nullptr;
	uint32_t                                  m_lastRendererGen    = 0;
};
