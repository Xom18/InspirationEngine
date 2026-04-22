#pragma once

#ifndef IE_LEGACY_TTF
// ──────────────────────────────────────────────
//  FreeType + HarfBuzz 렌더링 유틸리티
// ──────────────────────────────────────────────

#include <vector>
#include <unordered_map>

struct cShapedGlyph
{
	uint32_t glyphIndex;
	int      xAdvance;   // 26.6 fixed-point
	int      yAdvance;
	int      xOffset;
	int      yOffset;
	uint32_t cluster;    // 소스 UTF-8 바이트 오프셋
};

struct cTextMeasure
{
	int width;
	int height;
	int ascent;
	int descent;
};


class cTextRenderer
{
public:
	// HarfBuzz로 UTF-8 텍스트 쉐이핑
	static std::vector<cShapedGlyph> shape(
		hb_font_t*      hbFont,
		const char*     utf8Text,
		int             byteLen,
		hb_script_t     script    = HB_SCRIPT_UNKNOWN,
		hb_direction_t  direction = HB_DIRECTION_LTR);

	// 쉐이핑 결과로 픽셀 너비/높이 계산 (TTF_SizeUTF8 대체)
	static cTextMeasure measure(FT_Face face, const std::vector<cShapedGlyph>& glyphs);

	// 쉐이핑된 글리프를 SDL_Texture로 렌더링 (FT_RENDER_MODE_NORMAL, CPU 합성)
	// caller가 SDL_DestroyTexture 해야 함
	static SDL_Texture* renderToTexture(
		SDL_Renderer*                      renderer,
		FT_Face                            face,
		const std::vector<cShapedGlyph>&   glyphs,
		SDL_Color                          color,
		int                                w,
		int                                h,
		int                                ascent);
};

#endif // !IE_LEGACY_TTF
