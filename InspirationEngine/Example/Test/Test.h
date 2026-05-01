#pragma once
#include <iostream>
#include <string>

class Test
{
protected:
	int32_t     m_pass = 0;
	int32_t     m_fail = 0;
	const char* m_name = "";

	static std::string bytesToHex(const std::string& str);
	void checkStr(const char* label, const std::string& actual, const std::string& expected);
	void checkSize(const char* label, size_t actual, size_t expected);
	void printSection(const char* section) const;
	void printSummary() const;

public:
	virtual ~Test() = default;
	virtual void run() = 0;

	int32_t getPassCount() const { return m_pass; }
	int32_t getFailCount() const { return m_fail; }
};
