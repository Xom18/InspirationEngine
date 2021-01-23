#pragma once

//여러 창에서 폰트를 가져다 쓰는대 거기에 스타일 지정하면 문제가 생기는대
//이걸 뮤텍스 걸어서 해결하자니 한쪽이 다 처리할때까지 묶이는거라 속도 문제때문에 안함
//대신 스타일별로 폰트 만들어놓고 가져가서 쓰게 만듦
class cFont
{
private:
	std::map<int, TTF_Font*> m_mapTTF;

public:
	~cFont()
	{
		std::map<int, TTF_Font*>::iterator ite = m_mapTTF.begin();
		for(; ite != m_mapTTF.end(); ++ite)
			TTF_CloseFont(ite->second);
		m_mapTTF.clear();
	}

	void setTTF(int _iStyle, TTF_Font* _lpFont)
	{
		//이미 있으면 그거 지우고 그걸 대체, 없으면 추가
		std::map<int, TTF_Font*>::iterator ite = m_mapTTF.find(_iStyle);
		if(ite != m_mapTTF.end())
		{
			TTF_CloseFont(ite->second);
			ite->second = _lpFont;
			return;
		}

		m_mapTTF.insert(std::make_pair(_iStyle, _lpFont));
	}

	TTF_Font* get(int _iStyle = TTF_STYLE_NORMAL)
	{
		std::map<int, TTF_Font*>::iterator ite = m_mapTTF.find(_iStyle);
		if(ite != m_mapTTF.end())
			return ite->second;

		//없으면 스타일 없는 기본폰트 찾음
		ite = m_mapTTF.find(TTF_STYLE_NORMAL);
		if(ite != m_mapTTF.end())
			return ite->second;

		//그마저도 없으면 제일 앞에있는 폰트 가져옴
		ite = m_mapTTF.begin();
		if(ite != m_mapTTF.end())
			return ite->second;

		return nullptr;
	}
};

//폰트 관리하기 위해서 있는 클래스
class cFontManager
{
public:

private:
	std::map<int, cFont*> m_mapFont;

public:
	cFontManager();
	~cFontManager();

	bool addNewFont(int _iFontID, const char* _csFontDir, int _iFontSize, bool _bMakeOnlyDefault = true);
	bool addNewFont(int _iFontID, int _iStyle, TTF_Font* _lpFont);
	void setFont(int _iFontID, cFont* _lpFont)
	{
		//이미 있으면 그거 지우고 그걸 대체, 없으면 추가
		std::map<int, cFont*>::iterator ite = m_mapFont.find(_iFontID);
		if(ite != m_mapFont.end())
		{
			delete ite->second;
			ite->second = _lpFont;
		}

		m_mapFont.insert(std::make_pair(_iFontID, _lpFont));
	}

	cFont* getFont(int _iFontID)
	{
		std::map<int, cFont*>::iterator ite = m_mapFont.find(_iFontID);
		if(ite == m_mapFont.end())
			return nullptr;

		return ite->second;
	}

private:

};