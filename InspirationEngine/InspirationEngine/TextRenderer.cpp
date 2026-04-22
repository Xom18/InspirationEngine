#include "InspirationEngine.h"

#ifndef IE_LEGACY_TTF
// ──────────────────────────────────────────────
//  cTextRenderer
// ──────────────────────────────────────────────

std::vector<cShapedGlyph> cTextRenderer::shape(
	hb_font_t*     hbFont,
	const char*    utf8Text,
	int            byteLen,
	hb_script_t    script,
	hb_direction_t direction)
{
	hb_buffer_t* buf = hb_buffer_create();
	hb_buffer_add_utf8(buf, utf8Text, byteLen, 0, byteLen);
	hb_buffer_set_direction(buf, direction);

	if (script == HB_SCRIPT_UNKNOWN)
		hb_buffer_guess_segment_properties(buf);
	else
		hb_buffer_set_script(buf, script);

	hb_shape(hbFont, buf, nullptr, 0);

	uint32_t count = 0;
	hb_glyph_info_t*     info      = hb_buffer_get_glyph_infos(buf, &count);
	hb_glyph_position_t* positions = hb_buffer_get_glyph_positions(buf, &count);

	std::vector<cShapedGlyph> result(count);
	for (uint32_t i = 0; i < count; ++i)
	{
		result[i].glyphIndex = info[i].codepoint;
		result[i].xAdvance   = positions[i].x_advance;
		result[i].yAdvance   = positions[i].y_advance;
		result[i].xOffset    = positions[i].x_offset;
		result[i].yOffset    = positions[i].y_offset;
		result[i].cluster    = info[i].cluster;
	}

	hb_buffer_destroy(buf);
	return result;
}

cTextMeasure cTextRenderer::measure(FT_Face face, const std::vector<cShapedGlyph>& glyphs)
{
	int width = 0;
	for (const auto& g : glyphs)
		width += g.xAdvance >> 6;

	int ascent  =  (int)(face->size->metrics.ascender  >> 6);
	int descent = -(int)(face->size->metrics.descender >> 6);

	return { width, ascent + descent, ascent, descent };
}

SDL_Texture* cTextRenderer::renderToTexture(
	SDL_Renderer*                    renderer,
	FT_Face                          face,
	const std::vector<cShapedGlyph>& glyphs,
	SDL_Color                        color,
	int                              w,
	int                              h,
	int                              ascent)
{
	if (w <= 0 || h <= 0)
		return nullptr;

	// CPU 픽셀 버퍼에 합성 — 소프트웨어 렌더러에서도 동작
	std::vector<uint32_t> pixels((size_t)w * h, 0);

	int penX = 0;
	for (const auto& g : glyphs)
	{
		if (FT_Load_Glyph(face, g.glyphIndex, FT_LOAD_DEFAULT) != 0) { penX += g.xAdvance >> 6; continue; }
		if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) != 0) { penX += g.xAdvance >> 6; continue; }

		FT_Bitmap& bmp = face->glyph->bitmap;
		int drawX = penX + (g.xOffset >> 6) + face->glyph->bitmap_left;
		int drawY = ascent - face->glyph->bitmap_top + (g.yOffset >> 6);

		for (unsigned row = 0; row < bmp.rows; ++row)
		{
			int dstY = drawY + (int)row;
			if (dstY < 0 || dstY >= h) continue;
			for (unsigned col = 0; col < bmp.width; ++col)
			{
				int dstX = drawX + (int)col;
				if (dstX < 0 || dstX >= w) continue;

				uint8_t alpha = bmp.buffer[row * bmp.pitch + col];
				if (alpha == 0) continue;

				// ARGB8888
				uint32_t& dst = pixels[(size_t)dstY * w + dstX];
				// 알파 합성 (src over dst)
				uint32_t srcA = (uint32_t)alpha * color.a / 255;
				uint32_t dstA = (dst >> 24) & 0xFF;
				uint32_t outA = srcA + dstA * (255 - srcA) / 255;

				auto blend = [&](uint8_t sc, uint8_t dc) -> uint8_t {
					if (outA == 0) return 0;
					return (uint8_t)((sc * srcA + dc * dstA * (255 - srcA) / 255) / outA);
				};

				dst = (outA << 24)
					| ((uint32_t)blend(color.r, (dst >> 16) & 0xFF) << 16)
					| ((uint32_t)blend(color.g, (dst >>  8) & 0xFF) <<  8)
					| ((uint32_t)blend(color.b,  dst        & 0xFF));
			}
		}
		penX += g.xAdvance >> 6;
	}

	SDL_Texture* tex = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STATIC,
		w, h);

	if (!tex)
		return nullptr;

	SDL_UpdateTexture(tex, nullptr, pixels.data(), w * 4);
	SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
	return tex;
}

#endif // !IE_LEGACY_TTF
