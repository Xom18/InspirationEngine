#pragma once

class cDisplay	//화면 출력 클래스, 랜더 스레드화 필요
{
public:
	cDisplay(char* _lpTitle, int _Width, int _Height);
	~cDisplay();

	bool			closed();					//닫혔는지 확인
	SDL_Renderer*	getRenderer();				//랜더러 받아오기
	void			render();					//랜더
	void			draw(int* _lpBuffer, SDL_Rect _Rect);
	void			draw(int* _lpBuffer, int _iX, int _iY, int _iWidth, int _iHeight);
public:

private:
	SDL_Renderer*				m_Renderer;			//랜더러
	SDL_Window*					m_Window;			//창
	bool						m_IsRunning;		//도는중인지
	int							m_Width;
	int							m_Height;
};