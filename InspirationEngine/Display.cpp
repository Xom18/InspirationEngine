#include <SDL/SDL.h>
#include <iostream>
#include <vector>
#include "MacroDefine.h"
#include "Display.h"

cDisplay::cDisplay(char * _lpTitle, int _Width, int _Height, int _LayerCount)
{
	//SDL 초기화
	SDL_Init(SDL_INIT_VIDEO);
	//창 생성, 랜더러 생성, 드로우 레이어 생성, 버퍼 생성
	m_Window = SDL_CreateWindow(_lpTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _Width, _Height, 0);
	m_Renderer = SDL_CreateRenderer(m_Window, -1, 0);
	m_LayerCount = _LayerCount;
	m_pLayer.resize(_LayerCount);
	for(int i = 0; i < _LayerCount; ++i)
		m_pLayer[i] = SDL_CreateTexture(m_Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, _Width, _Height);
	
	//메모리 초기화
	m_pGraphicBuffer = new Uint32[_Width * _Height];
	memset(m_pGraphicBuffer, 255, _Width * _Height * sizeof(Uint32));
}

cDisplay::~cDisplay()
{
	KILL(m_pGraphicBuffer);
	for(int i = 0; i < m_LayerCount; ++i)
		SDL_DestroyTexture(m_pLayer[i]);
	SDL_DestroyRenderer(m_Renderer);
	SDL_DestroyWindow(m_Window);
}

SDL_Texture* cDisplay::getLayer(int _LayerIndex)
{
	return m_pLayer[_LayerIndex];
}

void cDisplay::render()
{
	for (int i = 0; i < m_LayerCount; ++i)
		SDL_RenderCopy(m_Renderer, m_pLayer[i], NULL, NULL);
	
	SDL_RenderPresent(m_Renderer);
}

bool cDisplay::closed()
{
	if (m_Window == NULL)
		return true;
	else
		return false;
}

int cDisplay::getLayerCount()
{
	return m_LayerCount;
}

Uint32* cDisplay::getGraphicBuffer()
{
	return m_pGraphicBuffer;
}

SDL_Renderer* cDisplay::getRenderer()
{
	return m_Renderer;
}