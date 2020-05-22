#pragma once

class cDisplay	//ȭ�� ��� Ŭ����, ���� ������ȭ �ʿ�
{
public:
	cDisplay(char* _lpTitle, int _Width, int _Height, int _LayerCount);
	~cDisplay();

	SDL_Texture*	getLayer(int _LayerIndex);	//���̾� �޾ƿ��°�
	bool			closed();					//�������� Ȯ��
	int				getLayerCount();			//���̾� ����
	SDL_Renderer*	getRenderer();				//������ �޾ƿ���
	void			render();					//����
private:
	Uint32*			getGraphicBuffer();			//�׷��� ���� �޾ƿ���

public:

private:
	Uint32*						m_pGraphicBuffer;	//���� ����
	SDL_Renderer*				m_Renderer;			//������
	SDL_Window*					m_Window;			//â
	std::vector<SDL_Texture*>	m_pLayer;			//���̾�
	int							m_LayerCount;		//���̾� ����
	bool						m_IsRunning;		//����������
};