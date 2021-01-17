#include "InspirationEngine.h"

void cTextBox::transToTexture()
{
	if(!m_lpRenderer)
		return;

	//이전에 이미 처리한적있는지 확인
	m_bTextChanged = false;
	size_t szHash = std::hash<std::string>{}(m_strText);
	if(m_szDrawHash == szHash)
		return;

	//텍스쳐 해제
	if(m_pTexture)
	{
		SDL_DestroyTexture(m_pTexture);
		m_pTexture = nullptr;
	}

	m_szDrawHash = szHash;

	SDL_Surface* pSurface = nullptr;
	pSurface = TTF_RenderUTF8_Solid(m_lpFont, m_strText.c_str(), m_Color);
	m_pTexture = SDL_CreateTextureFromSurface(m_lpRenderer->m_pRenderer, pSurface);
	SDL_FreeSurface(pSurface);
}

void cTextBox::draw()
{
	if(m_lpRenderer == nullptr)
		return;

	if(m_bTextChanged)
		transToTexture();

	m_lpRenderer->drawTexture(m_pTexture, m_rtRect.x, m_rtRect.y);
}

void cTextBox::update()
{

}