#include <fstream>
#include "Core/MacroDefine.h"
#include "IEFile.h"

IEFile::IEFile()
{
	reset();
}

IEFile::~IEFile()
{
	reset();
}

void IEFile::reset()
{
	pKILL(m_data);
	m_size = 0;
	m_cursor = 0;
}

bool IEFile::ReadFile(const char* filename)
{
	reset();
	//파일을 읽어들인다, (파일명, 파일형식)
	std::ifstream in(filename, std::ifstream::binary | std::ios::in);

	//만약 들어올 내용이 없으면 false 반환
	if (!in)
		return false;

	in.seekg(0, std::ifstream::end);//읽기 포인터의 위치를 파일의 맨 끝으로
	m_size = in.tellg();//파일 시작위치와 현재위치 사이의 길이<자료형>(파일의 총 길이) 구하기
	in.seekg(0, std::ifstream::beg);//다시 읽기 포인터를 맨 앞으로
	m_data = new char[static_cast<int32_t>(m_size)];//버퍼에 새로운 공간을 할당한다
	in.read(m_data, m_size);//읽는다

	return true;
}
