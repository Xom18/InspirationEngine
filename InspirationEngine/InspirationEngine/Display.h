#pragma once

class cDisplay	//화면 출력 클래스, 랜더 스레드화 필요
{
public:
	cDisplay();
	~cDisplay();

	bool			createWindow(char* _lpTitle, int _Width, int _Height, int _iRendererCount = 1);//창 생성
	bool			closed();	//닫혔는지 확인
	void			close();	//닫기
	void			resize();	//창 크기 변경
	void			reset();	//초기화
	SDL_Renderer*	getRenderer(int _iRendererIndex);//랜더러 받아오기
	SDL_Window*		getWindow();//창 받아오기
	void			render();	//랜더

	void			setRendererLogicalSize(int _iRendererIndex, int _iWidth, int _iHeight);	//논리적 랜더러 크기

	void			draw(int* _lpBuffer, int _iRendererIndex, const SDL_Rect& _Rect, SDL_BlendMode _BlendMode = SDL_BLENDMODE_BLEND);
	void			draw(int* _lpBuffer, int _iRendererIndex, int _iX, int _iY, int _iWidth, int _iHeight, SDL_BlendMode _BlendMode = SDL_BLENDMODE_BLEND);
	void			draw(SDL_Texture* _lpTexture, int _iRendererIndex, const SDL_Rect& _Rect);
	void			draw(SDL_Texture* _lpTexture, int _iRendererIndex, int _iX, int _iY, int _iWidth, int _iHeight);

public:

private:
	int							m_iRendererCount;	//랜더러 개수
	std::vector<SDL_Renderer*>	m_vecRenderer;		//소프트웨어 랜더러
	SDL_Window*					m_pWindow;			//창
	int							m_iWidth;			//너비
	int							m_iHeight;			//높이
};