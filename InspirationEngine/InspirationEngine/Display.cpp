#include <SDL/SDL.h>
#include <iostream>
#include <vector>
#include "MacroDefine.h"
#include "Display.h"

cDisplay::cDisplay(char * _lpTitle, int _Width, int _Height)
{
	//SDL 초기화
	SDL_Init(SDL_INIT_VIDEO);
	//창 생성, 랜더러 생성
	m_Window = SDL_CreateWindow(_lpTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _Width, _Height, 0);
	m_Renderer = SDL_CreateRenderer(m_Window, -1, 0);
	m_Width = _Width;
	m_Height = _Height;
}

cDisplay::~cDisplay()
{
	SDL_DestroyRenderer(m_Renderer);
	SDL_DestroyWindow(m_Window);
}

void cDisplay::render()
{
	SDL_RenderPresent(m_Renderer);
}

bool cDisplay::closed()
{
	if (m_Window == NULL)
		return true;
	else
		return false;
}

SDL_Renderer* cDisplay::getRenderer()
{
	return m_Renderer;
}

void cDisplay::draw(int* _lpBuffer, SDL_Rect _Rect)
{
	SDL_Texture* pTexture = SDL_CreateTexture(m_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, _Rect.w, _Rect.h);
	SDL_SetTextureBlendMode(pTexture, SDL_BLENDMODE_BLEND);
	SDL_UpdateTexture(pTexture, NULL, _lpBuffer, _Rect.w * sizeof(Uint32));
	SDL_RenderCopy(m_Renderer, pTexture, NULL, &_Rect);
	SDL_DestroyTexture(pTexture);
}

void cDisplay::draw(int* _lpBuffer, int _iX, int _iY, int _iWidth, int _iHeight)
{
	SDL_Rect Rect;//타일 띄울 위치
	Rect.x = _iX;
	Rect.y = _iY;
	Rect.w = _iWidth;
	Rect.h = _iHeight;
	draw(_lpBuffer, Rect);
}