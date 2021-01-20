#include "InspirationEngine.h"

void cTextBox::transToTexture()
{
	if(!m_lpRenderer)
		return;

	//이전에 이미 처리한적있는지 해시 비교로 확인
	m_bTextChanged = false;
	size_t szHash = std::hash<std::string>{}(m_strText);
	if(m_szDrawHash == szHash)
		return;

	//텍스쳐 해제
	resetTexture();

	//해시 갱신
	m_szDrawHash = szHash;

	//텍스트를 기반으로 텍스쳐화
	while(true)
	{
		SDL_Surface* pSurface = nullptr;
		pSurface = TTF_RenderUTF8_Solid(m_lpFont, m_strText.c_str(), m_Color);

		cTextTexture* pTTexture = new cTextTexture();
		pTTexture->m_pTexture = SDL_CreateTextureFromSurface(m_lpRenderer->m_pRenderer, pSurface);
		SDL_FreeSurface(pSurface);

		//텍스쳐 리스트에 추가
		m_listTexture.push_back(pTTexture);
		break;
	}
}

void cTextBox::draw()
{
	if(m_lpRenderer == nullptr)
		return;

	if(m_bTextChanged)
		transToTexture();

	std::list<cTextTexture*>::iterator ite = m_listTexture.begin();
	for(; ite != m_listTexture.end(); ++ite)
	{
		cTextTexture* pTTexture = *ite;
		m_lpRenderer->drawTexture(pTTexture->m_pTexture, m_rtRect.x + pTTexture->m_rtRect.x, m_rtRect.y + pTTexture->m_rtRect.y);
	}
}

void cTextBox::update()
{

}