#pragma once

class cDisplay	//화면 출력 클래스, 랜더 스레드화 필요
{
public:
	cDisplay(char* _lpTitle, int _Width, int _Height, int _LayerCount);
	~cDisplay();

	SDL_Texture*	getLayer(int _LayerIndex);	//레이어 받아오는거
	bool			closed();					//닫혔는지 확인
	int				getLayerCount();			//레이어 개수
	SDL_Renderer*	getRenderer();				//랜더러 받아오기
	void			render();					//랜더
private:
	Uint32*			getGraphicBuffer();			//그래픽 버퍼 받아오기

public:

private:
	Uint32*						m_pGraphicBuffer;	//랜더 버퍼
	SDL_Renderer*				m_Renderer;			//랜더러
	SDL_Window*					m_Window;			//창
	std::vector<SDL_Texture*>	m_pLayer;			//레이어
	int							m_LayerCount;		//레이어 개수
	bool						m_IsRunning;		//도는중인지
};