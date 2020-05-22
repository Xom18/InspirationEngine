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
	//������ �о���δ�, (���ϸ�, ��������)
	std::ifstream in(_lpFilename, std::ifstream::binary | std::ios::in);

	//���� ���� ������ ������ falase ��ȯ
	if (!in)
		return false;

	in.seekg(0, std::ifstream::end);//�б� �������� ��ġ�� ������ �� ������
	m_Size = (int)in.tellg();//���� ������ġ�� ������ġ ������ ����<�ڷ���>(������ �� ����) ���ϱ�
	in.seekg(0, std::ifstream::beg);//�ٽ� �б� �����͸� �� ������
	m_pData = new char[m_Size];//���ۿ� ���ο� ������ �Ҵ��Ѵ�
	in.read(m_pData, m_Size);//�д´�
	
	return true;
}