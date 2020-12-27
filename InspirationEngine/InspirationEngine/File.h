#pragma once

class cFile//파일 읽기 쓰기 관리하는부분
{
public:
	cFile();
	~cFile();
	bool ReadFile(char* _lpFilename);
	void reset();		//초기화

public:
	char*	m_pData;	//데이터 버퍼
	int		m_Size;		//데이터 크기

private:
	int		m_Cursor;	//읽을 때 쓰려고 만든 커서
};