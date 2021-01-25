#include "InspirationEngine.h"

cFontManager::cFontManager()
{

}

cFontManager::~cFontManager()
{
	std::map<int, cFont*>::iterator ite = m_mapFont.begin();
	for(; ite != m_mapFont.end(); ++ite)
		delete ite->second;
	m_mapFont.clear();
}

bool cFontManager::addNewFont(int _iFontID, const char* _csFontDir, int _iFontSize, bool _bMakeOnlyDefault)
{
//#define TTF_STYLE_BOLD          0x01
//#define TTF_STYLE_ITALIC        0x02
//#define TTF_STYLE_UNDERLINE     0x04
//#define TTF_STYLE_STRIKETHROUGH 0x08
	int iMakeFontCount = 1;
	if(_bMakeOnlyDefault == false)
		iMakeFontCount = 0x10;

	cFont* pFont = getFont(_iFontID);
	if(pFont == nullptr)
		pFont = new cFont();
	
	//폰트 스타일별로 바리에이션 자동생성
	for(int i = 0; i < iMakeFontCount; ++i)
	{
		//스타일별 폰트 생성
		TTF_Font* pTTF = TTF_OpenFont(_csFontDir, _iFontSize);
		if(pTTF == nullptr)
			continue;

		TTF_SetFontStyle(pTTF, i);
		pFont->setTTF(i, pTTF);
	}
	setFont(_iFontID, pFont);

	return true;
}

bool cFontManager::addNewFont(int _iFontID, int _iStyle, TTF_Font* _lpFont)
{
	cFont* pFont = getFont(_iFontID);
	if(pFont == nullptr)
		pFont = new cFont();

	pFont->setTTF(_iStyle, _lpFont);
	setFont(_iFontID, pFont);

	return true;
}