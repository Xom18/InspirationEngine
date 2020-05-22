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
	int imageToTile(cImage* _lpImage, int _SplitSize);	//이미지를 타일로 변환
	Uint32* getData();									//데이터 받아오기
	Uint32* getData(int _Index);							//특정 타일 데이터 받아오기

	int getTileCount();
	int getOffset();
protected:
	Uint32*	m_pData;		//타일 데이터
	int		m_TileCount;	//타일 개수
	int		m_Offset;		//타일 크기
private:
};