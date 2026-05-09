#include "InspirationEngine.h"

void IERenderer::drawRect(SDL_Color color, int32_t x, int32_t y, int32_t width, int32_t height, SDL_BlendMode blendMode, double angle, SDL_FPoint* pivot)
{
	SDL_Texture* pTexture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 1, 1);
	SDL_SetTextureBlendMode(pTexture, blendMode);

	Uint32 uiColor = 0;
	uiColor += color.a;
	uiColor <<= 8;
	uiColor += color.r;
	uiColor <<= 8;
	uiColor += color.g;
	uiColor <<= 8;
	uiColor += color.b;

	SDL_UpdateTexture(pTexture, NULL, &uiColor, sizeof(Uint32));

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

void IERenderer::drawBuffer(int32_t* buffer, int32_t bufferWidth, int32_t bufferHeight, int32_t x, int32_t y, SDL_BlendMode blendMode, double widthPercent, double heightPercent, double angle, SDL_FPoint* pivot, SDL_FlipMode flip)
{
	SDL_Texture* pTexture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, bufferWidth, bufferHeight);
	SDL_SetTextureBlendMode(pTexture, blendMode);
	SDL_UpdateTexture(pTexture, NULL, buffer, bufferWidth * sizeof(Uint32));
	drawTexture(pTexture, x, y, widthPercent, heightPercent, angle, pivot, flip);
	SDL_DestroyTexture(pTexture);
}

void IERenderer::drawTexture(SDL_Texture* texture, int32_t x, int32_t y, double widthPercent, double heightPercent, double angle, SDL_FPoint* pivot, SDL_FlipMode flip, const SDL_FRect* srcRect)
{
	//텍스쳐 정보 받아오기
	float fWidth = 0.0f;
	float fHeight = 0.0f;
	if (!SDL_GetTextureSize(texture, &fWidth, &fHeight))
		return;

	SDL_FRect SrcRect;
	SDL_FRect DestRect;
	DestRect.x = static_cast<float>(x);
	DestRect.y = static_cast<float>(y);

	if (srcRect)
	{
		SrcRect = *srcRect;
		DestRect.w = static_cast<float>(srcRect->w * widthPercent * 0.01);
		DestRect.h = static_cast<float>(srcRect->h * heightPercent * 0.01);
	}
	else
	{
		SrcRect = { 0.0f, 0.0f, fWidth, fHeight };
		DestRect.w = static_cast<float>((widthPercent != 100.0) ? fWidth * widthPercent * 0.01 : fWidth);
		DestRect.h = static_cast<float>((heightPercent != 100.0) ? fHeight * heightPercent * 0.01 : fHeight);
	}

	if (angle == 0.0 && flip == SDL_FLIP_NONE && pivot == nullptr)
	{
		bool isSuccess = SDL_RenderTexture(m_renderer, texture, &SrcRect, &DestRect);
		if (!isSuccess)
		{
			printf("WHAT");
		}
	}
	else
		SDL_RenderTextureRotated(m_renderer, texture, &SrcRect, &DestRect, angle, pivot, flip);
}

void IERenderer::drawSurface(SDL_Surface* surface, int32_t x, int32_t y, double widthPercent, double heightPercent, double angle, SDL_FPoint* pivot, SDL_FlipMode flip)
{
	SDL_Texture* pTexture = SDL_CreateTextureFromSurface(m_renderer, surface);
	drawTexture(pTexture, x, y, widthPercent, heightPercent, angle, pivot, flip);
	SDL_DestroyTexture(pTexture);
}

void IERenderer::drawLine(SDL_Color color, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderLine(m_renderer, static_cast<float>(x1), static_cast<float>(y1), static_cast<float>(x2), static_cast<float>(y2));
}

bool IERenderer::saveScreenshot(const char* path)
{
	SDL_Surface* surface = SDL_RenderReadPixels(m_renderer, nullptr);
	if (!surface) return false;

	bool ok = IMG_SavePNG(surface, path) == 0;
	SDL_DestroySurface(surface);
	return ok;
}

void IERenderer::drawText(IEFont* font, const char* text, SDL_Color color, int32_t x, int32_t y, double widthPercent, double heightPercent, double angle, SDL_FPoint* pivot, SDL_FlipMode flip)
{
	if (font == nullptr) return;
	IEFontFace* pFace = font->get();
	if (pFace == nullptr || pFace->m_ftFace == nullptr) return;

	auto glyphs = IETextRenderer::shape(pFace->m_hbFont, text, static_cast<int32_t>(std::strlen(text)));
	auto ms     = IETextRenderer::measure(pFace->m_ftFace, glyphs);

	if (ms.width <= 0 || ms.height <= 0) return;

	SDL_Texture* pTex = IETextRenderer::renderToTexture(
		m_renderer, pFace->m_ftFace, glyphs, color,
		ms.width, ms.height, ms.ascent, pFace->m_bold);

	if (pTex)
	{
		drawTexture(pTex, x, y, widthPercent, heightPercent, angle, pivot, flip);
		SDL_DestroyTexture(pTex);
	}
}
