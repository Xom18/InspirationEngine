#pragma once

class cFile//���� �б� ���� �����ϴºκ�
{
public:
	cFile();
	~cFile();
	bool ReadFile(char* _lpFilename);
	void reset();		//�ʱ�ȭ

public:
	char*	m_pData;	//������ ����
	int		m_Size;		//������ ũ��

private:
	int		m_Cursor;	//���� �� ������ ���� Ŀ��
};