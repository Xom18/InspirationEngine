#pragma once

class cImage
{
public:
	int readBMP(char* _Filename);
	cImage(char** _Buffer, int _Width, int _Height);
	cImage();
	~cImage();
	Uint32* getData();
	int getWidth();
	int getHeight();
protected:
	Uint32* m_pData;
	int m_Width;
	int m_Height;
private:
	
};

class cTile
{
public:
	cTile();
	~cTile();
	int imageToTile(cImage* _lpImage, int _SplitSize);	//�̹����� Ÿ�Ϸ� ��ȯ
	Uint32* getData();									//������ �޾ƿ���
	Uint32* getData(int _Index);							//Ư�� Ÿ�� ������ �޾ƿ���

	int getTileCount();
	int getOffset();
protected:
	Uint32*	m_pData;		//Ÿ�� ������
	int		m_TileCount;	//Ÿ�� ����
	int		m_Offset;		//Ÿ�� ũ��
private:
};