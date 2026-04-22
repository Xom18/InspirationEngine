#include <string>
#include <vector>
#include "StrUTF8.h"
#include "utf8proc.h"

bool cStrUTF8::IsValidStartByte(unsigned char c)
{
	if ((c & 0x80) == 0) return true;
	if ((c & 0xE0) == 0xC0) return c >= 0xC2;
	if ((c & 0xF0) == 0xE0) return true;
	if ((c & 0xF8) == 0xF0) return true;
	return false;
}

size_t cStrUTF8::GetUTF8ByteLength(unsigned char c)
{
	if ((c & 0x80) == 0) return 1;
	if ((c & 0xE0) == 0xC0) return 2;
	if ((c & 0xF0) == 0xE0) return 3;
	if ((c & 0xF8) == 0xF0) return 4;
	return 1;
}

void cStrUTF8::pop_back(std::string& inputStr)
{
	if (inputStr.empty())
		return;

	const auto* u = (const utf8proc_uint8_t*)inputStr.data();
	utf8proc_ssize_t len = (utf8proc_ssize_t)inputStr.size();

	// 앞에서 스캔해 마지막 코드포인트 시작 위치를 찾는다
	utf8proc_ssize_t lastStart = 0, i = 0;
	while (i < len) {
		utf8proc_int32_t cp;
		utf8proc_ssize_t n = utf8proc_iterate(u + i, len - i, &cp);
		if (n <= 0) n = 1;
		lastStart = i;
		i += n;
	}
	inputStr.erase((size_t)lastStart);
}

void cStrUTF8::pop_front(std::string& inputStr)
{
	if (inputStr.empty())
		return;

	utf8proc_int32_t cp;
	utf8proc_ssize_t n = utf8proc_iterate(
		(const utf8proc_uint8_t*)inputStr.data(),
		(utf8proc_ssize_t)inputStr.size(), &cp);
	inputStr.erase(0, (size_t)(n > 0 ? n : 1));
}

size_t cStrUTF8::getMemoryPoint(std::string& inputStr, size_t _szPoint)
{
	if (_szPoint == 0)
		return 0;

	const auto* u = (const utf8proc_uint8_t*)inputStr.data();
	utf8proc_ssize_t len = (utf8proc_ssize_t)inputStr.size();
	utf8proc_ssize_t i = 0;
	size_t chars = 0;

	while (i < len && chars < _szPoint) {
		utf8proc_int32_t cp;
		utf8proc_ssize_t n = utf8proc_iterate(u + i, len - i, &cp);
		i += (n > 0) ? n : 1;
		++chars;
	}

	if (chars < _szPoint)
		return std::string::npos;

	return (size_t)i;
}

void cStrUTF8::removeToFront(std::string& inputStr, size_t _szPoint, size_t _szCount)
{
	if (_szPoint == 0 || inputStr.empty())
		return;

	if (_szPoint > inputStr.size())
		_szPoint = inputStr.size();

	const auto* u = (const utf8proc_uint8_t*)inputStr.data();
	utf8proc_ssize_t limit = (utf8proc_ssize_t)_szPoint;

	// _szPoint 이전 코드포인트 시작 위치 목록 수집
	std::vector<utf8proc_ssize_t> starts;
	utf8proc_ssize_t i = 0;
	while (i < limit) {
		starts.push_back(i);
		utf8proc_int32_t cp;
		utf8proc_ssize_t n = utf8proc_iterate(u + i, limit - i, &cp);
		i += (n > 0) ? n : 1;
	}

	if (starts.empty())
		return;

	size_t eraseFrom = (starts.size() <= _szCount)
		? (size_t)starts[0]
		: (size_t)starts[starts.size() - _szCount];

	inputStr.erase(eraseFrom, _szPoint - eraseFrom);
}

void cStrUTF8::removeToBack(std::string& inputStr, size_t _szPoint, size_t _szCount)
{
	size_t iLength = inputStr.length();
	if (iLength == 0 || _szPoint >= iLength)
		return;

	const auto* u = (const utf8proc_uint8_t*)inputStr.data();
	utf8proc_ssize_t len = (utf8proc_ssize_t)iLength;
	utf8proc_ssize_t i = (utf8proc_ssize_t)_szPoint;
	size_t removed = 0;

	while (i < len && removed < _szCount) {
		utf8proc_int32_t cp;
		utf8proc_ssize_t n = utf8proc_iterate(u + i, len - i, &cp);
		i += (n > 0) ? n : 1;
		++removed;
	}

	inputStr.erase(_szPoint, (size_t)i - _szPoint);
}
