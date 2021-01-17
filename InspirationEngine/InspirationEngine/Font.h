#pragma once

//폰트 관리하기 위해서 있는 클래스
class cFontManager
{
public:

private:
	std::map<int, TTF_Font*> m_mapFont;

public:
	cFontManager();
	~cFontManager();

	bool addNewFont(int _iFontID, const char* _csFontDir, int _iFontSize);
	bool addNewFont(int _iFontID, TTF_Font* _lpFont);
	
	TTF_Font* getFont(int _iFontID)
	{
		std::map<int, TTF_Font*>::iterator ite = m_mapFont.find(_iFontID);
		if(ite == m_mapFont.end())
			return nullptr;

		return ite->second;
	}

private:

};