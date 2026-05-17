#pragma once
#include <vector>

/// <summary>
/// 파일 읽기 관리
/// </summary>
class IEFile
{
public:
	IEFile();
	~IEFile();

	/// <summary>
	/// 파일을 읽어 m_data에 저장 — 성공하면 true
	/// </summary>
	/// <param name="filename">읽을 파일 경로</param>
	bool ReadFile(const char* filename);

	/// <summary>
	/// m_data 해제 및 크기/커서 초기화
	/// </summary>
	void Reset();

private:
	std::vector<char> m_data;
	int32_t           m_cursor = 0;
};
