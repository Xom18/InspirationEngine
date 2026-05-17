#include "InspirationEngine.h"

void IERenderer::DrawRect(SDL_Color color, int32_t x, int32_t y, int32_t width, int32_t height, SDL_BlendMode blendMode, double angle, SDL_FPoint* pivot)
{
	SDL_Texture* pTexture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 1, 1);
	SDL_SetTextureScaleMode(pTexture, SDL_SCALEMODE_NEAREST);
	SDL_SetTextureBlendMode(pTexture, blendMode);

	Uint32 colorValue = 0;
	colorValue += color.a;
	colorValue <<= 8;
	colorValue += color.r;
	colorValue <<= 8;
	colorValue += color.g;
	colorValue <<= 8;
	colorValue += color.b;

	SDL_UpdateTexture(pTexture, nullptr,&colorValue, sizeof(Uint32));

	//텍스쳐 내에서의 위치
	SDL_FRect SrcRect;
	SrcRect.x = 0.0f;
	SrcRect.y = 0.0f;
	SrcRect.w = 1.0f;
	SrcRect.h = 1.0f;

	//화면에서의 위치
	SDL_FRect DestRect;
	DestRect.x = static_cast<float>(x);
	DestRect.y = static_cast<float>(y);
	DestRect.w = static_cast<float>(width);
	DestRect.h = static_cast<float>(height);

	SDL_RenderTextureRotated(m_renderer, pTexture, &SrcRect, &DestRect, angle, pivot, SDL_FLIP_NONE);

	SDL_DestroyTexture(pTexture);
}

void IERenderer::DrawBuffer(int32_t* buffer, int32_t bufferWidth, int32_t bufferHeight, int32_t x, int32_t y, SDL_BlendMode blendMode, double widthPercent, double heightPercent, double angle, SDL_FPoint* pivot, SDL_FlipMode flip)
{
	SDL_Texture* pTexture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, bufferWidth, bufferHeight);
	SDL_SetTextureScaleMode(pTexture, SDL_SCALEMODE_PIXELART);
	SDL_SetTextureBlendMode(pTexture, blendMode);
	SDL_UpdateTexture(pTexture, nullptr,buffer, bufferWidth * sizeof(Uint32));
	DrawTexture(pTexture, x, y, widthPercent, heightPercent, angle, pivot, flip);
	SDL_DestroyTexture(pTexture);
}

void IERenderer::DrawTexture(SDL_Texture* texture, int32_t x, int32_t y, double widthPercent, double heightPercent, double angle, SDL_FPoint* pivot, SDL_FlipMode flip, const SDL_FRect* srcRect)
{
	//텍스쳐 정보 받아오기
	float texWidth = 0.0f;
	float texHeight = 0.0f;
	if (!SDL_GetTextureSize(texture, &texWidth, &texHeight))
		return;

	SDL_FRect SrcRect;
	SDL_FRect DestRect;
	DestRect.x = static_cast<float>(x);
	DestRect.y = static_cast<float>(y);

	if (srcRect != nullptr)
	{
		SrcRect = *srcRect;
		DestRect.w = static_cast<float>(srcRect->w * widthPercent * 0.01);
		DestRect.h = static_cast<float>(srcRect->h * heightPercent * 0.01);
	}
	else
	{
		SrcRect = { 0.0f, 0.0f, texWidth, texHeight };
		DestRect.w = static_cast<float>((widthPercent != 100.0) ? texWidth * widthPercent * 0.01 : texWidth);
		DestRect.h = static_cast<float>((heightPercent != 100.0) ? texHeight * heightPercent * 0.01 : texHeight);
	}

	if (angle == 0.0 && flip == SDL_FLIP_NONE && pivot == nullptr)
		SDL_RenderTexture(m_renderer, texture, &SrcRect, &DestRect);
	else
		SDL_RenderTextureRotated(m_renderer, texture, &SrcRect, &DestRect, angle, pivot, flip);
}

void IERenderer::DrawSurface(SDL_Surface* surface, int32_t x, int32_t y, double widthPercent, double heightPercent, double angle, SDL_FPoint* pivot, SDL_FlipMode flip)
{
	SDL_Texture* pTexture = SDL_CreateTextureFromSurface(m_renderer, surface);
	SDL_SetTextureScaleMode(pTexture, SDL_SCALEMODE_PIXELART);
	DrawTexture(pTexture, x, y, widthPercent, heightPercent, angle, pivot, flip);
	SDL_DestroyTexture(pTexture);
}

void IERenderer::DrawTextureRect(SDL_Texture* texture, int32_t x, int32_t y, int32_t w, int32_t h)
{
	float texW = 0.0f, texH = 0.0f;
	if (!SDL_GetTextureSize(texture, &texW, &texH))
		return;

	SDL_FRect src = { 0.0f, 0.0f, texW, texH };
	SDL_FRect dst = {
		static_cast<float>(x), static_cast<float>(y),
		static_cast<float>(w), static_cast<float>(h)
	};
	SDL_RenderTexture(m_renderer, texture, &src, &dst);
}

void IERenderer::DrawSurfaceRect(SDL_Surface* surface, int32_t x, int32_t y, int32_t w, int32_t h)
{
	SDL_Texture* tex = SDL_CreateTextureFromSurface(m_renderer, surface);
	if (tex == nullptr)
		return;

	SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_LINEAR);
	DrawTextureRect(tex, x, y, w, h);
	SDL_DestroyTexture(tex);
}

void IERenderer::DrawLine(SDL_Color color, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderLine(m_renderer, static_cast<float>(x1), static_cast<float>(y1), static_cast<float>(x2), static_cast<float>(y2));
}

bool IERenderer::SaveScreenshot(const char* path)
{
	SDL_Surface* surface = SDL_RenderReadPixels(m_renderer, nullptr);
	if (surface == nullptr)
		return false;

	bool ok = IMG_SavePNG(surface, path) == 0;
	SDL_DestroySurface(surface);
	return ok;
}

void IERenderer::DrawText(IEFont* font, const char* text, SDL_Color color, int32_t x, int32_t y, double widthPercent, double heightPercent, double angle, SDL_FPoint* pivot, SDL_FlipMode flip)
{
	if (font == nullptr)
		return;
	IEFontFace* pFace = font->Get();
	if (pFace == nullptr || pFace->m_ftFace == nullptr)
		return;

	// FT_Face / hb_font_t 는 스레드 비안전: shape + measure + renderToTexture 전 구간 직렬화
	std::vector<IEShapedGlyph> glyphs;
	IETextMeasure ms = {};
	SDL_Texture* pTex = nullptr;
	{
		std::lock_guard<std::mutex> lock(pFace->m_mutex);

		glyphs = IETextRenderer::shape(pFace->m_hbFont, text, static_cast<int32_t>(std::strlen(text)));
		ms     = IETextRenderer::measure(pFace->m_ftFace, glyphs);

		if (ms.width <= 0 || ms.height <= 0)
			return;

		pTex = IETextRenderer::renderToTexture(
			m_renderer, pFace->m_ftFace, glyphs, color,
			ms.width, ms.height, ms.ascent, pFace->m_bold);
	}

	if (pTex != nullptr)
	{
		DrawTexture(pTex, x, y, widthPercent, heightPercent, angle, pivot, flip);
		SDL_DestroyTexture(pTex);
	}
}
