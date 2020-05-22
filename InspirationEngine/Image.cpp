#include <SDL/SDL.h>
#include "MacroDefine.h"
#include "Image.h"
#include "File.h"

cImage::cImage()
{
	m_pData = NULL;
	m_Width = 0;
	m_Height = 0;
}

cImage::cImage(char** buffer, int width, int height)
{
	m_pData = NULL;
	m_Width = 0;
	m_Height = 0;
}

cImage::~cImage()
{

}

//RGBA BMP only
int cImage::readBMP(char* filename)
{
	static char Magic[3] = { 'B', 'M', '6' };	//BMP파일 앞쪽에 오는 식별자

	aKILL(m_pData);		
	m_Width = 0;
	m_Height = 0;

	Uint32* Buffer_int;
	int Offset;
	char Bm6[3];

	cFile File;
	File.ReadFile(filename);

	//파일 읽어서 BMP인지 구분자가지고 비교
	for (int i = 0; i < 3; ++i)
		Bm6[i] = File.m_pData[i];

	for(int i = 0; i < 3; ++i)
		if (Bm6[i] != Magic[i])
			return 1;

	Buffer_int = reinterpret_cast<Uint32*>(File.m_pData + 18);
	Offset = *(File.m_pData + 10);
	m_Width = *(Buffer_int);
	m_Height = *(Buffer_int + 1);

	Buffer_int = reinterpret_cast<Uint32*>(File.m_pData + Offset - 1);
	m_pData = new Uint32[m_Width * m_Height];

	for (int y = 0; y < m_Height; ++y)
	{
		for (int x = 0; x < m_Width; ++x)
		{
			m_pData[y * m_Width + x] = Buffer_int[(m_Height - y - 1) * m_Width + x];
		}
	}

	return 0;
}

Uint32* cImage::getData()
{
	return m_pData;
}

int cImage::getWidth()
{
	return m_Width;
}

int cImage::getHeight()
{
	return m_Height;
}

cTile::cTile()
{

}

cTile::~cTile()
{

}

//ex)64*64타일을 16*256처럼 잘라주는 함수
int cTile::imageToTile(cImage* _lpImage, int _SplitSize)
{
	int ImageWidth = _lpImage->getWidth();
	int ImageHeight = _lpImage->getHeight();
	Uint32* lpBuffer = _lpImage->getData();
	if (ImageWidth == 0 || ImageHeight == 0)
		return 1;

	aKILL(m_pData);
	m_pData = new Uint32[ImageWidth * ImageHeight];
	int counter = 0;
	int splitWidth = ImageWidth / _SplitSize;
	int splitHeight = ImageHeight / _SplitSize;
	m_TileCount = _SplitSize * splitHeight;
	m_Offset = _SplitSize * _SplitSize;

	for (int y = 0; y < splitHeight; ++y)
	{
		for (int x = 0; x < splitWidth; ++x)
		{
			for (int Y = 0; Y < _SplitSize; ++Y)
			{
				for (int X = 0; X < _SplitSize; ++X)
				{
					m_pData[counter] = lpBuffer[(y * _SplitSize + Y) * _lpImage->getWidth() + x * _SplitSize + X];
					++counter;
				}
			}
		}
	}
	return 0;
}

Uint32* cTile::getData()
{
	return m_pData;
}

Uint32* cTile::getData(int _Index)
{
	return m_pData + _Index * m_Offset;
}

int cTile::getTileCount()
{
	return m_TileCount;
}

int cTile::getOffset()
{
	return m_Offset;
}