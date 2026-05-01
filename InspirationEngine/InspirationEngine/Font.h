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

struct cFontFace
{
	FT_Face    ftFace = nullptr;
	hb_font_t* hbFont = nullptr;
	bool       bBold  = false;

	~cFontFace()
	{
		if (hbFont) { hb_font_destroy(hbFont); hbFont = nullptr; }
		if (ftFace) { FT_Done_Face(ftFace);    ftFace = nullptr; }
	}
};

class cFont
{
private:
	std::map<int, cFontFace*> m_mapFace;

public:
	~cFont()
	{
		for (auto& [_, f] : m_mapFace) delete f;
		m_mapFace.clear();
	}

	void setFace(int _iStyle, cFontFace* _lpFace)
	{
		auto ite = m_mapFace.find(_iStyle);
		if (ite != m_mapFace.end())
		{
			delete ite->second;
			ite->second = _lpFace;
			return;
		}
		m_mapFace.insert({ _iStyle, _lpFace });
	}

	cFontFace* get(int _iStyle = IE_FONT_STYLE_NORMAL)
	{
		auto ite = m_mapFace.find(_iStyle);
		if (ite != m_mapFace.end()) return ite->second;

		ite = m_mapFace.find(IE_FONT_STYLE_NORMAL);
		if (ite != m_mapFace.end()) return ite->second;

		if (!m_mapFace.empty()) return m_mapFace.begin()->second;
		return nullptr;
	}

	int getHeight() const
	{
		auto ite = m_mapFace.find(IE_FONT_STYLE_NORMAL);
		if (ite == m_mapFace.end() || !ite->second->ftFace) return 0;
		return (int)(ite->second->ftFace->size->metrics.height >> 6);
	}

	int getAscent() const
	{
		auto ite = m_mapFace.find(IE_FONT_STYLE_NORMAL);
		if (ite == m_mapFace.end() || !ite->second->ftFace) return 0;
		return (int)(ite->second->ftFace->size->metrics.ascender >> 6);
	}

	int getDescent() const
	{
		auto ite = m_mapFace.find(IE_FONT_STYLE_NORMAL);
		if (ite == m_mapFace.end() || !ite->second->ftFace) return 0;
		return -(int)(ite->second->ftFace->size->metrics.descender >> 6);
	}
};

class cFontManager
{
private:
	FT_Library m_ftLib = nullptr;
	std::map<int, cFont*> m_mapFont;

public:
	cFontManager()  { FT_Init_FreeType(&m_ftLib); }
	~cFontManager()
	{
		for (auto& [_, f] : m_mapFont) delete f;
		m_mapFont.clear();
		if (m_ftLib) { FT_Done_FreeType(m_ftLib); m_ftLib = nullptr; }
	}

	bool addNewFont(int _iFontID, const char* _csFontDir, int _iFontSize, bool _bMakeOnlyDefault = false);
	FT_Library getFTLib() { return m_ftLib; }

	void setFont(int _iFontID, cFont* _lpFont)
	{
		auto ite = m_mapFont.find(_iFontID);
		if (ite != m_mapFont.end()) { delete ite->second; ite->second = _lpFont; return; }
		m_mapFont.insert({ _iFontID, _lpFont });
	}

	cFont* getFont(int _iFontID)
	{
		auto ite = m_mapFont.find(_iFontID);
		return (ite != m_mapFont.end()) ? ite->second : nullptr;
	}
};
