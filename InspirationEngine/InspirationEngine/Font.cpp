#include "InspirationEngine.h"

#ifdef IE_LEGACY_TTF
// ──────────────────────────────────────────────
//  레거시 경로 (SDL2_ttf)
// ──────────────────────────────────────────────

cFontManager::cFontManager()
{
}

cFontManager::~cFontManager()
{
	std::map<int, cFont*>::iterator ite = m_mapFont.begin();
	for (; ite != m_mapFont.end(); ++ite)
		delete ite->second;
	m_mapFont.clear();
}

bool cFontManager::addNewFont(int _iFontID, const char* _csFontDir, int _iFontSize, bool _bMakeOnlyDefault)
{
	//#define TTF_STYLE_BOLD          0x01
	//#define TTF_STYLE_ITALIC        0x02
	//#define TTF_STYLE_UNDERLINE     0x04
	//#define TTF_STYLE_STRIKETHROUGH 0x08
	int iMakeFontCount = 1;
	if (_bMakeOnlyDefault == false)
		iMakeFontCount = 0x10;

	cFont* pFont = getFont(_iFontID);
	if (pFont == nullptr)
		pFont = new cFont();

	for (int i = 0; i < iMakeFontCount; ++i)
	{
		TTF_Font* pTTF = TTF_OpenFont(_csFontDir, _iFontSize);
		if (pTTF == nullptr)
			continue;

		TTF_SetFontStyle(pTTF, i);
		pFont->setTTF(i, pTTF);
	}
	setFont(_iFontID, pFont);

	return true;
}

bool cFontManager::addNewFont(int _iFontID, int _iStyle, TTF_Font* _lpFont)
{
	cFont* pFont = getFont(_iFontID);
	if (pFont == nullptr)
		pFont = new cFont();

	pFont->setTTF(_iStyle, _lpFont);
	setFont(_iFontID, pFont);

	return true;
}

#else
// ──────────────────────────────────────────────
//  새 렌더링 경로 (FreeType + HarfBuzz)
// ──────────────────────────────────────────────

bool cFontManager::addNewFont(int _iFontID, const char* _csFontDir, int _iFontSize, bool _bMakeOnlyDefault)
{
	int iMakeFontCount = _bMakeOnlyDefault ? 1 : 0x10;

	cFont* pFont = getFont(_iFontID);
	if (pFont == nullptr)
		pFont = new cFont();

	for (int i = 0; i < iMakeFontCount; ++i)
	{
		FT_Face face = nullptr;
		if (FT_New_Face(m_ftLib, _csFontDir, 0, &face) != 0)
			continue;

		FT_Set_Pixel_Sizes(face, 0, (FT_UInt)_iFontSize);

		// Bold 합성 (bit 0x01)
		if (i & IE_FONT_STYLE_BOLD)
			FT_GlyphSlot_Embolden(face->glyph);

		// Italic 합성 (bit 0x02)
		if (i & IE_FONT_STYLE_ITALIC)
		{
			FT_Matrix matrix = { 0x10000, 0x3000, 0, 0x10000 };
			FT_Set_Transform(face, &matrix, nullptr);
		}

		cFontFace* pFace = new cFontFace();
		pFace->ftFace = face;

		// hb_ft_font_create 미사용 — blob 경로로 face/font 생성
		hb_blob_t* hbBlob = hb_blob_create_from_file(_csFontDir);
		hb_face_t* hbFace = hb_face_create(hbBlob, 0);
		hb_blob_destroy(hbBlob);
		pFace->hbFont = hb_font_create(hbFace);
		hb_face_destroy(hbFace);
		// 26.6 픽셀 단위 advance 반환하도록 스케일 설정
		hb_font_set_scale(pFace->hbFont,
			(int)face->size->metrics.x_ppem * 64,
			(int)face->size->metrics.y_ppem * 64);

		pFont->setFace(i, pFace);
	}
	setFont(_iFontID, pFont);

	return true;
}

#endif // IE_LEGACY_TTF
