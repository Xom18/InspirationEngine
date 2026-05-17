#include <string>
#include <vector>
#include "IEStrUTF8.h"
#include "utf8proc.h"

bool IEStrUTF8::IsValidStartByte(uint8_t c)
{
	if ((c & 0x80) == 0)
		return true;
	if ((c & 0xE0) == 0xC0)
		return c >= 0xC2;
	if ((c & 0xF0) == 0xE0)
		return true;
	if ((c & 0xF8) == 0xF0)
		return true;
	return false;
}

size_t IEStrUTF8::GetUTF8ByteLength(uint8_t c)
{
	if ((c & 0x80) == 0)
		return 1;
	if ((c & 0xE0) == 0xC0)
		return 2;
	if ((c & 0xF0) == 0xE0)
		return 3;
	if ((c & 0xF8) == 0xF0)
		return 4;
	return 1;
}

void IEStrUTF8::PopBack(std::string& inputStr)
{
	if (inputStr.empty())
		return;

	const auto* u = reinterpret_cast<const utf8proc_uint8_t*>(inputStr.data());
	utf8proc_ssize_t len = static_cast<utf8proc_ssize_t>(inputStr.size());

	// 앞에서 스캔해 마지막 코드포인트 시작 위치를 찾는다
	utf8proc_ssize_t lastStart = 0, i = 0;
	while (i < len) {
		utf8proc_int32_t cp;
		utf8proc_ssize_t n = utf8proc_iterate(u + i, len - i, &cp);
		if (n <= 0)
			n = 1;
		lastStart = i;
		i += n;
	}
	inputStr.erase(static_cast<size_t>(lastStart));
}

void IEStrUTF8::PopFront(std::string& inputStr)
{
	if (inputStr.empty())
		return;

	utf8proc_int32_t cp;
	utf8proc_ssize_t n = utf8proc_iterate(
		reinterpret_cast<const utf8proc_uint8_t*>(inputStr.data()),
		static_cast<utf8proc_ssize_t>(inputStr.size()), &cp);
	inputStr.erase(0, static_cast<size_t>(n > 0 ? n : 1));
}

size_t IEStrUTF8::GetMemoryPoint(std::string& str, size_t point)
{
	if (point == 0)
		return 0;

	const auto* u = reinterpret_cast<const utf8proc_uint8_t*>(str.data());
	utf8proc_ssize_t len = static_cast<utf8proc_ssize_t>(str.size());
	utf8proc_ssize_t i = 0;
	size_t chars = 0;

	while (i < len && chars < point) {
		utf8proc_int32_t cp;
		utf8proc_ssize_t n = utf8proc_iterate(u + i, len - i, &cp);
		i += (n > 0) ? n : 1;
		++chars;
	}

	if (chars < point)
		return std::string::npos;

	return static_cast<size_t>(i);
}

void IEStrUTF8::RemoveToFront(std::string& str, size_t point, size_t count)
{
	if (point == 0 || str.empty())
		return;

	if (point > str.size())
		point = str.size();

	const auto* u = reinterpret_cast<const utf8proc_uint8_t*>(str.data());
	utf8proc_ssize_t limit = static_cast<utf8proc_ssize_t>(point);

	// point 이전 코드포인트 시작 위치 목록 수집
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

	size_t eraseFrom = (starts.size() <= count)
		? static_cast<size_t>(starts[0])
		: static_cast<size_t>(starts[starts.size() - count]);

	str.erase(eraseFrom, point - eraseFrom);
}

void IEStrUTF8::RemoveToBack(std::string& str, size_t point, size_t count)
{
	size_t iLength = str.length();
	if (iLength == 0 || point >= iLength)
		return;

	const auto* u = reinterpret_cast<const utf8proc_uint8_t*>(str.data());
	utf8proc_ssize_t len = static_cast<utf8proc_ssize_t>(iLength);
	utf8proc_ssize_t i = static_cast<utf8proc_ssize_t>(point);
	size_t removed = 0;

	while (i < len && removed < count) {
		utf8proc_int32_t cp;
		utf8proc_ssize_t n = utf8proc_iterate(u + i, len - i, &cp);
		i += (n > 0) ? n : 1;
		++removed;
	}

	str.erase(point, static_cast<size_t>(i) - point);
}
