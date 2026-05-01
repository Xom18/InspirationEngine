#pragma once

/// <summary>
/// 파일 읽기 관리
/// </summary>
class File
{
public:
	char* m_data;		//데이터 버퍼
	std::streamsize	m_size;		//데이터 크기

private:
	int32_t		m_cursor;	//읽을 때 쓰려고 만든 커서

public:
	File();
	~File();

	/// <summary>
	/// 파일을 읽어 m_data에 저장 — 성공하면 true
	/// </summary>
	/// <param name="filename">읽을 파일 경로</param>
	bool ReadFile(const char* filename);

	/// <summary>
	/// m_data 해제 및 크기/커서 초기화
	/// </summary>
	void reset();

private:

};
