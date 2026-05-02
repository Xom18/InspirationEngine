#include "InspirationEngine.h"

void IERenderer::drawRect(SDL_Color color, int32_t x, int32_t y, int32_t width, int32_t height, SDL_BlendMode blendMode, double angle, SDL_Point* pivot)
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
	SDL_Rect SrcRect;
	SrcRect.x = 0;
	SrcRect.y = 0;
	SrcRect.w = 1;
	SrcRect.h = 1;

	//화면에서의 위치
	SDL_Rect DestRect;
	DestRect.x = x;
	DestRect.y = y;
	DestRect.w = width;
	DestRect.h = height;

	SDL_RenderCopyEx(m_renderer, pTexture, &SrcRect, &DestRect, angle, pivot, SDL_FLIP_NONE);

	SDL_DestroyTexture(pTexture);
}

void IERenderer::drawBuffer(int32_t* buffer, int32_t bufferWidth, int32_t bufferHeight, int32_t x, int32_t y, SDL_BlendMode blendMode, double widthPercent, double heightPercent, double angle, SDL_Point* pivot, SDL_RendererFlip flip)
{
	SDL_Texture* pTexture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, bufferWidth, bufferHeight);
	SDL_SetTextureBlendMode(pTexture, blendMode);
	SDL_UpdateTexture(pTexture, NULL, buffer, bufferWidth * sizeof(Uint32));
	drawTexture(pTexture, x, y, widthPercent, heightPercent, angle, pivot, flip);
	SDL_DestroyTexture(pTexture);
}

void IERenderer::drawTexture(SDL_Texture* texture, int32_t x, int32_t y, double widthPercent, double heightPercent, double angle, SDL_Point* pivot, SDL_RendererFlip flip, const SDL_Rect* srcRect)
{
	//텍스쳐 정보 받아오기
	int32_t iWidth = 0;
	int32_t iHeight = 0;
	if (SDL_QueryTexture(texture, NULL, NULL, &iWidth, &iHeight) == -1)
		return;

	SDL_Rect SrcRect;
	SDL_Rect DestRect;
	DestRect.x = x;
	DestRect.y = y;

	if (srcRect)
	{
		SrcRect = *srcRect;
		DestRect.w = static_cast<int32_t>(srcRect->w * widthPercent * 0.01);
		DestRect.h = static_cast<int32_t>(srcRect->h * heightPercent * 0.01);
	}
	else
	{
		SrcRect = { 0, 0, iWidth, iHeight };
		DestRect.w = (widthPercent != 100.0) ? static_cast<int32_t>(iWidth * widthPercent * 0.01) : iWidth;
		DestRect.h = (heightPercent != 100.0) ? static_cast<int32_t>(iHeight * heightPercent * 0.01) : iHeight;
	}

	SDL_RenderCopyEx(m_renderer, texture, &SrcRect, &DestRect, angle, pivot, flip);
}

void IERenderer::drawSurface(SDL_Surface* surface, int32_t x, int32_t y, double widthPercent, double heightPercent, double angle, SDL_Point* pivot, SDL_RendererFlip flip)
{
	SDL_Texture* pTexture = SDL_CreateTextureFromSurface(m_renderer, surface);
	drawTexture(pTexture, x, y, widthPercent, heightPercent, angle, pivot, flip);
	SDL_DestroyTexture(pTexture);
}

void IERenderer::drawText(Font* font, const char* text, SDL_Color color, int32_t x, int32_t y, double widthPercent, double heightPercent, double angle, SDL_Point* pivot, SDL_RendererFlip flip)
{
	if (font == nullptr) return;
	FontFace* pFace = font->get();
	if (pFace == nullptr || pFace->ftFace == nullptr) return;

	auto glyphs = IETextRenderer::shape(pFace->hbFont, text, static_cast<int32_t>(std::strlen(text)));
	auto ms     = IETextRenderer::measure(pFace->ftFace, glyphs);

	if (ms.width <= 0 || ms.height <= 0) return;

	SDL_Texture* pTex = IETextRenderer::renderToTexture(
		m_renderer, pFace->ftFace, glyphs, color,
		ms.width, ms.height, ms.ascent, pFace->bBold);

	if (pTex)
	{
		drawTexture(pTex, x, y, widthPercent, heightPercent, angle, pivot, flip);
		SDL_DestroyTexture(pTex);
	}
}
