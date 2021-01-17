#include "InspirationEngine.h"

cFontManager::cFontManager()
{

}

cFontManager::~cFontManager()
{
	std::map<int, TTF_Font*>::iterator ite = m_mapFont.begin();
	for(; ite != m_mapFont.end(); ++ite)
		TTF_CloseFont(ite->second);
	m_mapFont.clear();
}

bool cFontManager::addNewFont(int _iFontID, const char* _csFontDir, int _iFontSize)
{
	if(getFont(_iFontID) != nullptr)
		return false;

	//폰트 생성
	TTF_Font* pFont = TTF_OpenFont(_csFontDir, _iFontSize);
	if(pFont == nullptr)
		return false;

	//추가 실패
	if(!addNewFont(_iFontID, pFont))
	{
		//폰트 제거
		TTF_CloseFont(pFont);
		return false;
	}

	return true;
}

bool cFontManager::addNewFont(int _iFontID, TTF_Font* _lpFont)
{
	if(getFont(_iFontID) != nullptr)
		return false;

	 m_mapFont.insert(std::pair<int, TTF_Font*>(_iFontID, _lpFont));

	return true;
}