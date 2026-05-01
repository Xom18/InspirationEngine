#pragma once

#ifndef IE_LEGACY_TTF
// ──────────────────────────────────────────────
//  FreeType + HarfBuzz 렌더링 유틸리티
// ──────────────────────────────────────────────

#include <vector>
#include <unordered_map>

struct ShapedGlyph
{
	uint32_t glyphIndex;
	int32_t  xAdvance;   // 26.6 fixed-point
	int32_t  yAdvance;
	int32_t  xOffset;
	int32_t  yOffset;
	uint32_t cluster;    // 소스 UTF-8 바이트 오프셋
};

struct TextMeasure
{
	int32_t width;
	int32_t height;
	int32_t ascent;
	int32_t descent;
};


class IETextRenderer
{
public:
	/// <summary>
	/// HarfBuzz로 UTF-8 텍스트 쉐이핑
	/// </summary>
	/// <param name="hbFont">HarfBuzz 폰트</param>
	/// <param name="utf8Text">UTF-8 텍스트</param>
	/// <param name="byteLen">바이트 길이</param>
	/// <param name="script">스크립트 (기본값: 자동 감지)</param>
	/// <param name="direction">텍스트 방향</param>
	static std::vector<ShapedGlyph> shape(
		hb_font_t*      hbFont,
		const char*     utf8Text,
		int32_t         byteLen,
		hb_script_t     script    = HB_SCRIPT_UNKNOWN,
		hb_direction_t  direction = HB_DIRECTION_LTR);

	/// <summary>
	/// 쉐이핑 결과로 픽셀 너비/높이 계산
	/// </summary>
	/// <param name="face">FreeType 폰트 페이스</param>
	/// <param name="glyphs">쉐이핑된 글리프 목록</param>
	static TextMeasure measure(FT_Face face, const std::vector<ShapedGlyph>& glyphs);

	/// <summary>
	/// 쉐이핑된 글리프를 SDL_Texture로 렌더링 — 호출자가 SDL_DestroyTexture 해야 함
	/// </summary>
	/// <param name="renderer">SDL 렌더러</param>
	/// <param name="face">FreeType 폰트 페이스</param>
	/// <param name="glyphs">쉐이핑된 글리프 목록</param>
	/// <param name="color">글자 색상</param>
	/// <param name="w">텍스쳐 너비</param>
	/// <param name="h">텍스쳐 높이</param>
	/// <param name="ascent">어센트 (px)</param>
	/// <param name="bBold">볼드 합성 여부</param>
	static SDL_Texture* renderToTexture(
		SDL_Renderer*                      renderer,
		FT_Face                            face,
		const std::vector<ShapedGlyph>&    glyphs,
		SDL_Color                          color,
		int32_t                            w,
		int32_t                            h,
		int32_t                            ascent,
		bool                               bBold = false);
};

#endif // !IE_LEGACY_TTF
