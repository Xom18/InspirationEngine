#include "InspirationEngine.h"

// ──────────────────────────────────────────────
//  FreeType + HarfBuzz
// ──────────────────────────────────────────────

bool IEFontManager::addNewFont(int32_t fontID, const char* fontDir, int32_t fontSize, bool makeOnlyDefault)
{
	int32_t iMakeFontCount = makeOnlyDefault ? 1 : 0x10;

	Font* pFont = getFont(fontID);
	if (pFont == nullptr)
		pFont = new Font();

	for (int32_t i = 0; i < iMakeFontCount; ++i)
	{
		FT_Face face = nullptr;
		if (FT_New_Face(m_ftLib, fontDir, 0, &face) != 0)
			continue;

		FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(fontSize));

		// Italic 합성 (bit 0x02)
		if (i & IE_FONT_STYLE_ITALIC)
		{
			FT_Matrix matrix = { 0x10000, 0x3000, 0, 0x10000 };
			FT_Set_Transform(face, &matrix, nullptr);
		}

		FontFace* pFace = new FontFace();
		pFace->m_ftFace = face;
		pFace->m_bold   = (i & IE_FONT_STYLE_BOLD) != 0;

		// hb_ft_font_create 미사용 — blob 경로로 face/font 생성
		hb_blob_t* hbBlob = hb_blob_create_from_file(fontDir);
		hb_face_t* hbFace = hb_face_create(hbBlob, 0);
		hb_blob_destroy(hbBlob);
		pFace->m_hbFont = hb_font_create(hbFace);
		hb_face_destroy(hbFace);
		// 26.6 픽셀 단위 advance 반환하도록 스케일 설정
		hb_font_set_scale(pFace->m_hbFont,
			static_cast<int32_t>(face->size->metrics.x_ppem) * 64,
			static_cast<int32_t>(face->size->metrics.y_ppem) * 64);

		pFont->setFace(i, pFace);
	}
	setFont(fontID, pFont);

	return true;
}
