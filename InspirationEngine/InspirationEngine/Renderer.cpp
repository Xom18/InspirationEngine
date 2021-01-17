#include "InspirationEngine.h"

void cRenderer::drawBuffer(int* _lpBuffer, int _iBufferWidth, int _iBufferHeight, int _iX, int _iY, SDL_BlendMode _BlendMode, double _dWidthPercent, double _dHeightPercent, double _dAngle, SDL_Point* _lpPivot, SDL_RendererFlip _Flip)
{
	SDL_Texture* pTexture = SDL_CreateTexture(m_pRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, _iBufferWidth, _iBufferHeight);
	SDL_SetTextureBlendMode(pTexture, _BlendMode);
	SDL_UpdateTexture(pTexture, NULL, _lpBuffer, _iBufferWidth * sizeof(Uint32));
	drawTexture(pTexture, _iX, _iY, _dWidthPercent, _dHeightPercent, _dAngle, _lpPivot, _Flip);
	SDL_DestroyTexture(pTexture);
}

void cRenderer::drawTexture(SDL_Texture* _lpTexture, int _iX, int _iY, double _dWidthPercent, double _dHeightPercent, double _dAngle, SDL_Point* _lpPivot, SDL_RendererFlip _Flip)
{
	//텍스쳐 정보 받아오기
	int iWidth = 0;
	int iHeight = 0;
	if(SDL_QueryTexture(_lpTexture, NULL, NULL, &iWidth, &iHeight) == -1)
		return;

	//텍스쳐 내에서의 위치
	SDL_Rect SrcRect;
	SrcRect.x = 0;
	SrcRect.y = 0;
	SrcRect.w = iWidth;
	SrcRect.h = iHeight;

	//화면에서의 위치
	SDL_Rect DestRect;
	DestRect.x = _iX;
	DestRect.y = _iY;
	DestRect.w = iWidth;
	DestRect.h = iHeight;
	if(_dWidthPercent != 100.0)
		DestRect.w = static_cast<int>(iWidth * _dWidthPercent * 0.01);
	if(_dHeightPercent != 100.0)
		DestRect.h = static_cast<int>(iHeight * _dHeightPercent * 0.01);

	SDL_RenderCopyEx(m_pRenderer, _lpTexture, &SrcRect, &DestRect, _dAngle, _lpPivot, _Flip);
}

void cRenderer::drawSurface(SDL_Surface* _lpSurface, int _iX, int _iY, double _dWidthPercent, double _dHeightPercent, double _dAngle, SDL_Point* _lpPivot, SDL_RendererFlip _Flip)
{
	SDL_Texture* pTexture = SDL_CreateTextureFromSurface(m_pRenderer, _lpSurface);
	drawTexture(pTexture, _iX, _iY, _dWidthPercent, _dHeightPercent, _dAngle, _lpPivot, _Flip);
	SDL_DestroyTexture(pTexture);
}

void cRenderer::drawText(TTF_Font* _lpFont, const char* _lpText, SDL_Color _Color, int _iX, int _iY, double _dWidthPercent, double _dHeightPercent, double _dAngle, SDL_Point* _lpPivot, SDL_RendererFlip _Flip)
{
	SDL_Surface* pSurface = nullptr;
	pSurface = TTF_RenderUTF8_Solid(_lpFont, _lpText, _Color);
	drawSurface(pSurface, _iX, _iY, _dWidthPercent, _dHeightPercent, _dAngle, _lpPivot, _Flip);
	SDL_FreeSurface(pSurface);
}
