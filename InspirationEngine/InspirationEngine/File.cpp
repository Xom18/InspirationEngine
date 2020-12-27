#include <SDL/SDL.h>
#include <fstream>
#include "MacroDefine.h"
#include "File.h"

cFile::cFile()
{
	reset();
}

cFile::~cFile()
{
	reset();
}

void cFile::reset()
{
	aKILL(m_pData);
	m_Size = 0;
	m_Cursor = 0;
}

bool cFile::ReadFile(char* _lpFilename)
{
	reset();
	//파일을 읽어들인다, (파일명, 파일형식)
	std::ifstream in(_lpFilename, std::ifstream::binary | std::ios::in);

	//만약 들어올 내용이 없으면 false 반환
	if (!in)
		return false;

	in.seekg(0, std::ifstream::end);//읽기 포인터의 위치를 파일의 맨 끝으로
	m_Size = (int)in.tellg();//파일 시작위치와 현재위치 사이의 길이<자료형>(파일의 총 길이) 구하기
	in.seekg(0, std::ifstream::beg);//다시 읽기 포인터를 맨 앞으로
	m_pData = new char[m_Size];//버퍼에 새로운 공간을 할당한다
	in.read(m_pData, m_Size);//읽는다
	
	return true;
}