#include "Test.h"
#include <cstdio>

std::string Test::bytesToHex(const std::string& str)
{
	std::string hex;
	for (unsigned char c : str)
	{
		char buf[3];
		snprintf(buf, sizeof(buf), "%02X", c);
		hex += buf;
	}
	return hex;
}

void Test::checkStr(const char* label, const std::string& actual, const std::string& expected)
{
	if (actual == expected)
	{
		std::cout << "  PASS  " << label << std::endl;
		++m_pass;
	}
	else
	{
		std::cout << "  FAIL  " << label << std::endl;
		std::cout << "         got:      " << bytesToHex(actual) << std::endl;
		std::cout << "         expected: " << bytesToHex(expected) << std::endl;
		++m_fail;
	}
}

void Test::checkSize(const char* label, size_t actual, size_t expected)
{
	if (actual == expected)
	{
		std::cout << "  PASS  " << label << std::endl;
		++m_pass;
	}
	else
	{
		std::cout << "  FAIL  " << label
			<< ": got " << actual << ", expected " << expected << std::endl;
		++m_fail;
	}
}

void Test::printSection(const char* section) const
{
	std::cout << "=== " << section << " ===" << std::endl;
}

void Test::printSummary() const
{
	std::cout << "============================" << std::endl;
	std::cout << "PASS: " << m_pass
		<< "  FAIL: " << m_fail
		<< "  TOTAL: " << (m_pass + m_fail) << std::endl;
	if (m_fail == 0)
		std::cout << "All tests passed!" << std::endl;
	std::cout << std::endl;
}
