#include "InspirationEngine.h"

// ──────────────────────────────────────────────
//  FreeType + HarfBuzz
// ──────────────────────────────────────────────

#ifdef _WIN32
static const char* kSystemFontCandidates[] = {
    "C:\\Windows\\Fonts\\segoeui.ttf",  // Segoe UI — Vista 이후 전 지역 포함
    nullptr
};
#endif

// fontDir 로드 실패 시 시스템 폰트 경로를 순서대로 시도, 성공한 경로 반환. 전부 실패면 빈 문자열.
static std::string ResolveFont(FT_Library lib, const char* fontDir)
{
    FT_Face face = nullptr;
    if (FT_New_Face(lib, fontDir, 0, &face) == 0)
    {
        FT_Done_Face(face);
        return fontDir;
    }

#ifdef _WIN32
    for (int32_t i = 0; kSystemFontCandidates[i] != nullptr; ++i)
    {
        if (FT_New_Face(lib, kSystemFontCandidates[i], 0, &face) == 0)
        {
            FT_Done_Face(face);
            return kSystemFontCandidates[i];
        }
    }
#endif

    return {};
}

bool IEFontManager::AddNewFont(int32_t fontID, const char* fontDir, int32_t fontSize, bool makeOnlyDefault)
{
    std::string resolved = ResolveFont(m_ftLib, fontDir);
    if (resolved.empty())
        return false;

    const char* actualDir     = resolved.c_str();
    int32_t     makeFontCount = makeOnlyDefault ? 1 : 0x10;

    IEFont* pFont = GetFont(fontID);
    if (pFont == nullptr)
        pFont = new IEFont();

    for (int32_t i = 0; i < makeFontCount; ++i)
    {
        FT_Face face = nullptr;
        if (FT_New_Face(m_ftLib, actualDir, 0, &face) != 0)
            continue;

        FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(fontSize));

        if (i & IE_FONT_STYLE_ITALIC)
        {
            FT_Matrix matrix = { 0x10000, 0x3000, 0, 0x10000 };
            FT_Set_Transform(face, &matrix, nullptr);
        }

        IEFontFace* pFace   = new IEFontFace();
        pFace->m_ftFace     = face;
        pFace->m_bold       = (i & IE_FONT_STYLE_BOLD) != 0;

        hb_blob_t* hbBlob   = hb_blob_create_from_file(actualDir);
        hb_face_t* hbFace   = hb_face_create(hbBlob, 0);
        hb_blob_destroy(hbBlob);
        pFace->m_hbFont     = hb_font_create(hbFace);
        hb_face_destroy(hbFace);
        hb_font_set_scale(pFace->m_hbFont,
            static_cast<int32_t>(face->size->metrics.x_ppem) * 64,
            static_cast<int32_t>(face->size->metrics.y_ppem) * 64);

        pFont->SetFace(i, pFace);
    }
    SetFont(fontID, pFont);

    return true;
}
