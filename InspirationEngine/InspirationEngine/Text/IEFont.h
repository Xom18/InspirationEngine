#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_SYNTHESIS_H
#include <hb.h>

#define IE_FONT_STYLE_NORMAL        0x00
#define IE_FONT_STYLE_BOLD          0x01
#define IE_FONT_STYLE_ITALIC        0x02
#define IE_FONT_STYLE_UNDERLINE     0x04
#define IE_FONT_STYLE_STRIKETHROUGH 0x08

struct IEFontFace
{
	FT_Face    m_ftFace = nullptr;
	hb_font_t* m_hbFont = nullptr;
	bool       m_bold   = false;

	~IEFontFace()
	{
		if (m_hbFont != nullptr)
		{
			hb_font_destroy(m_hbFont);
			m_hbFont = nullptr;
		}
		if (m_ftFace != nullptr)
		{
			FT_Done_Face(m_ftFace);
			m_ftFace = nullptr;
		}
	}
};

/// <summary>
/// 하나의 폰트 파일에서 생성된 스타일별 IEFontFace 묶음
/// </summary>
class IEFont
{
public:
	/// <summary>
	/// 스타일에 맞는 폰트 페이스 등록 — 이미 있으면 교체
	/// </summary>
	/// <param name="style">IE_FONT_STYLE_* 플래그 조합</param>
	/// <param name="face">등록할 IEFontFace (소유권 이전)</param>
	void SetFace(int32_t style, IEFontFace* face)
	{
		auto ite = m_faces.find(style);
		if (ite != m_faces.end())
		{
			ite->second.reset(face);
			return;
		}
		m_faces.try_emplace(style, face);
	}

	/// <summary>
	/// 스타일에 맞는 폰트 페이스 반환 — 없으면 NORMAL, 그것도 없으면 첫 번째 페이스로 폴백
	/// </summary>
	/// <param name="style">IE_FONT_STYLE_* 플래그 조합</param>
	IEFontFace* Get(int32_t style = IE_FONT_STYLE_NORMAL)
	{
		auto ite = m_faces.find(style);
		if (ite != m_faces.end())
			return ite->second.get();

		ite = m_faces.find(IE_FONT_STYLE_NORMAL);
		if (ite != m_faces.end())
			return ite->second.get();

		if (!m_faces.empty())
			return m_faces.begin()->second.get();
		return nullptr;
	}

	/// <summary>
	/// NORMAL 스타일 기준 폰트 높이 반환 (px)
	/// </summary>
	int32_t GetHeight() const
	{
		auto ite = m_faces.find(IE_FONT_STYLE_NORMAL);
		if (ite == m_faces.end() || ite->second->m_ftFace == nullptr)
			return 0;
		return static_cast<int32_t>(ite->second->m_ftFace->size->metrics.height >> 6);
	}

	/// <summary>
	/// NORMAL 스타일 기준 어센트 반환 (px)
	/// </summary>
	int32_t GetAscent() const
	{
		auto ite = m_faces.find(IE_FONT_STYLE_NORMAL);
		if (ite == m_faces.end() || ite->second->m_ftFace == nullptr)
			return 0;
		return static_cast<int32_t>(ite->second->m_ftFace->size->metrics.ascender >> 6);
	}

	/// <summary>
	/// NORMAL 스타일 기준 디센트 반환 (px, 양수)
	/// </summary>
	int32_t GetDescent() const
	{
		auto ite = m_faces.find(IE_FONT_STYLE_NORMAL);
		if (ite == m_faces.end() || ite->second->m_ftFace == nullptr)
			return 0;
		return -static_cast<int32_t>(ite->second->m_ftFace->size->metrics.descender >> 6);
	}

private:
	std::map<int32_t, std::unique_ptr<IEFontFace>> m_faces;
};

class IEFontManager
{
public:
	IEFontManager()  { FT_Init_FreeType(&m_ftLib); }
	~IEFontManager()
	{
		m_fonts.clear();
		if (m_ftLib != nullptr)
		{
			FT_Done_FreeType(m_ftLib);
			m_ftLib = nullptr;
		}
	}

	/// <summary>
	/// 폰트 파일을 로드해 스타일별 FontFace를 생성하고 등록
	/// </summary>
	/// <param name="fontID">등록할 폰트 ID</param>
	/// <param name="fontDir">폰트 파일 경로</param>
	/// <param name="fontSize">픽셀 크기</param>
	/// <param name="makeOnlyDefault">true면 NORMAL 스타일만 생성</param>
	bool AddNewFont(int32_t fontID, const char* fontDir, int32_t fontSize, bool makeOnlyDefault = false);

	/// <summary>
	/// FreeType 라이브러리 포인터 반환
	/// </summary>
	FT_Library GetFTLib() { return m_ftLib; }

	/// <summary>
	/// ID에 IEFont 등록 — 이미 있으면 교체
	/// </summary>
	/// <param name="fontID">등록할 폰트 ID</param>
	/// <param name="font">등록할 IEFont (소유권 이전)</param>
	void SetFont(int32_t fontID, IEFont* font)
	{
		auto ite = m_fonts.find(fontID);
		if (ite != m_fonts.end())
		{
			ite->second.reset(font);
			return;
		}
		m_fonts.try_emplace(fontID, font);
	}

	/// <summary>
	/// ID로 IEFont 반환 — 없으면 nullptr
	/// </summary>
	/// <param name="fontID">찾을 폰트 ID</param>
	IEFont* GetFont(int32_t fontID)
	{
		auto ite = m_fonts.find(fontID);
		return (ite != m_fonts.end()) ? ite->second.get() : nullptr;
	}

private:
	FT_Library                                 m_ftLib = nullptr;
	std::map<int32_t, std::unique_ptr<IEFont>> m_fonts;
};
