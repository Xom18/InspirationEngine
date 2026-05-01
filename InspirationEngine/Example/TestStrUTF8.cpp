#include <iostream>
#include <string>
#include <cstdio>
#include "../InspirationEngine/utf8proc/utf8proc.h"
#include "../InspirationEngine/StrUTF8.h"

static int g_pass = 0;
static int g_fail = 0;

static std::string BytesToHex(const std::string& str)
{
	std::string hex;
	for (unsigned char c : str) {
		char buf[3];
		sprintf_s(buf, sizeof(buf), "%02X", c);
		hex += buf;
	}
	return hex;
}

static void CHECK_STR(const char* label, const std::string& actual, const std::string& expected)
{
	if (actual == expected) {
		std::cout << "  PASS  " << label << std::endl;
		++g_pass;
	} else {
		std::cout << "  FAIL  " << label << std::endl;
		std::cout << "         got:      " << BytesToHex(actual) << std::endl;
		std::cout << "         expected: " << BytesToHex(expected) << std::endl;
		++g_fail;
	}
}

static void CHECK_SIZE(const char* label, size_t actual, size_t expected)
{
	if (actual == expected) {
		std::cout << "  PASS  " << label << std::endl;
		++g_pass;
	} else {
		std::cout << "  FAIL  " << label
			<< ": got " << actual << ", expected " << expected << std::endl;
		++g_fail;
	}
}

// ── Test 1: 단독 연속 바이트 (0x80~0xBF) ───────────────────────────
static void TestInvalidContinuationByte()
{
	std::cout << "=== Test 1: Invalid Continuation Byte ===" << std::endl;

	// pop_back: "Hello\x80World" → 'd' 제거
	std::string s1 = "Hello\x80World";
	cStrUTF8::pop_back(s1);
	CHECK_STR("pop_back (invalid mid)", s1, "Hello\x80" "Worl");

	// pop_front: "\x80Hello" → 0x80(1B) 제거 → "Hello"
	std::string s2 = "\x80Hello";
	cStrUTF8::pop_front(s2);
	CHECK_STR("pop_front (leading 0x80)", s2, "Hello");

	std::cout << std::endl;
}

// ── Test 2: 불완전 멀티바이트 ──────────────────────────────────────
static void TestIncompleteMutliByte()
{
	std::cout << "=== Test 2: Incomplete Multi-byte ===" << std::endl;

	// "Hello\xC0" — 0xC0 단독: 오류→1B 취급, getMemoryPoint(1)=1
	std::string s1 = "Hello\xC0";
	CHECK_SIZE("getMemoryPoint(1) 'Hello\\xC0'", cStrUTF8::getMemoryPoint(s1, 1), 1u);

	// "Hi\xE0\x80" — 불완전 3B 시퀀스: 각 1B 취급, getMemoryPoint(1)=1
	std::string s2 = "Hi\xE0\x80";
	CHECK_SIZE("getMemoryPoint(1) 'Hi\\xE0\\x80'", cStrUTF8::getMemoryPoint(s2, 1), 1u);

	std::cout << std::endl;
}

// ── Test 3: 잘못된 시작 바이트 (0xF5+) ───────────────────────────
static void TestInvalidStartByte()
{
	std::cout << "=== Test 3: Invalid Start Byte ===" << std::endl;

	// 0xF5 이상은 UTF-8 무효 → 1B 취급
	std::string s = "Test\xF5\x80\x80\x80" "End";
	CHECK_SIZE("getMemoryPoint(2) 'Test\\xF5...'", cStrUTF8::getMemoryPoint(s, 2), 2u);

	std::cout << std::endl;
}

// ── Test 4: 정상 UTF-8 (ASCII+한국어) ────────────────────────────
static void TestValidUTF8()
{
	std::cout << "=== Test 4: Valid UTF-8 ===" << std::endl;

	// "Hello가나다": H(0)e(1)l(2)l(3)o(4)가(5)나(8)다(11)
	std::string s = "Hello\xEA\xB0\x80\xEB\x82\xB0\xEB\x8B\xA0";
	CHECK_SIZE("getMemoryPoint(6) → byte 8 (나)", cStrUTF8::getMemoryPoint(s, 6), 8u);

	// pop_back: 다(3B) 제거 → 11B
	std::string t = s;
	cStrUTF8::pop_back(t);
	CHECK_STR("pop_back removes 다", t, "Hello\xEA\xB0\x80\xEB\x82\xB0");

	std::cout << std::endl;
}

// ── Test 5: removeToFront/removeToBack + 잘못된 바이트 ───────────
static void TestRemoveWithInvalid()
{
	std::cout << "=== Test 5: Remove with Invalid Bytes ===" << std::endl;

	// "\x80Hello" removeToFront(2,1): 바이트 2 이전 문자 1개 삭제 → 'H' 제거
	std::string s1 = "\x80Hello";
	cStrUTF8::removeToFront(s1, 2, 1);
	CHECK_STR("removeToFront(2,1) '\\x80Hello'", s1, "\x80" "ello");

	// "Hello\x80" removeToBack(5,1): 바이트 5부터 1개 삭제 → 0x80 제거
	std::string s2 = "Hello\x80";
	cStrUTF8::removeToBack(s2, 5, 1);
	CHECK_STR("removeToBack(5,1) 'Hello\\x80'", s2, "Hello");

	std::cout << std::endl;
}

// ── Test 6: pop_back/pop_front + 잘못된 바이트 ───────────────────
static void TestPopWithInvalid()
{
	std::cout << "=== Test 6: Pop with Invalid Bytes ===" << std::endl;

	// "Test\x80" pop_back → 0x80 제거
	std::string s1 = "Test\x80";
	cStrUTF8::pop_back(s1);
	CHECK_STR("pop_back 'Test\\x80'", s1, "Test");

	// "\x80Test" pop_front → 0x80 제거
	std::string s2 = "\x80Test";
	cStrUTF8::pop_front(s2);
	CHECK_STR("pop_front '\\x80Test'", s2, "Test");

	std::cout << std::endl;
}

// ── Test 7: Overlong encoding (0xC0 0x80) ─────────────────────────
static void TestOverlongEncoding()
{
	std::cout << "=== Test 7: Overlong Encoding ===" << std::endl;

	// 0xC0 0x80 → overlong, utf8proc 오류 → 각 1B 취급
	std::string s = "Test\xC0\x80" "End";
	CHECK_SIZE("getMemoryPoint(1) 'Test\\xC0\\x80End'", cStrUTF8::getMemoryPoint(s, 1), 1u);

	std::cout << std::endl;
}

// ── Test 8: 연속된 잘못된 바이트 ─────────────────────────────────
static void TestConsecutiveInvalidBytes()
{
	std::cout << "=== Test 8: Consecutive Invalid Bytes ===" << std::endl;

	std::string s = "Start\x80\x81\x82" "End";
	CHECK_SIZE("getMemoryPoint(2) 'Start\\x80\\x81\\x82End'", cStrUTF8::getMemoryPoint(s, 2), 2u);

	// removeToBack(5,3): 잘못된 3바이트 제거 → "StartEnd"
	std::string t = s;
	cStrUTF8::removeToBack(t, 5, 3);
	CHECK_STR("removeToBack(5,3) removes 3 invalid bytes", t, "StartEnd");

	std::cout << std::endl;
}

// ── Test 9: CJK (한자·일본어·혼합) ───────────────────────────────
static void TestCJK()
{
	std::cout << "=== Test 9: CJK Characters ===" << std::endl;

	// 你好世界: 각 3B, 총 12B
	std::string chinese =
		"\xE4\xBD\xA0"   // 你
		"\xE5\xA5\xBD"   // 好
		"\xE4\xB8\x96"   // 世
		"\xE7\x95\x8C";  // 界

	CHECK_SIZE("getMemoryPoint(2) 你好→ byte 6", cStrUTF8::getMemoryPoint(chinese, 2), 6u);

	std::string t = chinese;
	cStrUTF8::pop_back(t);
	CHECK_STR("pop_back removes 界(3B)",
		t, "\xE4\xBD\xA0\xE5\xA5\xBD\xE4\xB8\x96");

	t = chinese;
	cStrUTF8::pop_front(t);
	CHECK_STR("pop_front removes 你(3B)",
		t, "\xE5\xA5\xBD\xE4\xB8\x96\xE7\x95\x8C");

	t = chinese;
	cStrUTF8::removeToFront(t, 6, 2);
	CHECK_STR("removeToFront(6,2) removes 你好 → 世界",
		t, "\xE4\xB8\x96\xE7\x95\x8C");

	// 日本語: 각 3B
	std::string japanese = "\xE6\x97\xA5\xE6\x9C\xAC\xE8\xAA\x9E";
	CHECK_SIZE("getMemoryPoint(1) 日→ byte 3", cStrUTF8::getMemoryPoint(japanese, 1), 3u);

	// "Hi你好": H(1)i(1)你(3)好(3) = 8B
	std::string mixed = "Hi\xE4\xBD\xA0\xE5\xA5\xBD";
	CHECK_SIZE("getMemoryPoint(3) Hi你→ byte 5", cStrUTF8::getMemoryPoint(mixed, 3), 5u);

	t = mixed;
	cStrUTF8::pop_back(t);
	CHECK_STR("pop_back removes 好(3B) → Hi你(5B)",
		t, "Hi\xE4\xBD\xA0");

	std::cout << std::endl;
}

// ── Test 10: 아랍어 (2B UTF-8, RTL) ──────────────────────────────
static void TestArabic()
{
	std::cout << "=== Test 10: Arabic Characters ===" << std::endl;

	// مرحبا: 각 2B, 총 10B
	std::string arabic =
		"\xD9\x85"   // م
		"\xD8\xB1"   // ر
		"\xD8\xAD"   // ح
		"\xD8\xA8"   // ب
		"\xD8\xA7";  // ا

	CHECK_SIZE("getMemoryPoint(2) مر→ byte 4", cStrUTF8::getMemoryPoint(arabic, 2), 4u);

	std::string t = arabic;
	cStrUTF8::pop_back(t);
	CHECK_STR("pop_back removes ا(2B)",
		t, "\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8");

	t = arabic;
	cStrUTF8::pop_front(t);
	CHECK_STR("pop_front removes م(2B)",
		t, "\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7");

	t = arabic;
	cStrUTF8::removeToBack(t, 4, 2);
	CHECK_STR("removeToBack(4,2) removes حب → مرا(6B)",
		t, "\xD9\x85\xD8\xB1\xD8\xA7");

	// مَر: م(D985) + fatha(D98E) + ر(D8B1) = 6B, 3 codepoints
	// pop_back은 코드포인트 단위 → ر(2B) 제거 → م + fatha(4B)
	std::string withDiacritic = "\xD9\x85\xD9\x8E\xD8\xB1";
	t = withDiacritic;
	cStrUTF8::pop_back(t);
	CHECK_STR("pop_back on مَر removes ر → مَ(4B)",
		t, "\xD9\x85\xD9\x8E");

	std::cout << std::endl;
}

void TestStrUTF8()
{
	std::cout << "===== UTF-8 Test Suite =====" << std::endl << std::endl;

	TestInvalidContinuationByte();
	TestIncompleteMutliByte();
	TestInvalidStartByte();
	TestValidUTF8();
	TestRemoveWithInvalid();
	TestPopWithInvalid();
	TestOverlongEncoding();
	TestConsecutiveInvalidBytes();
	TestCJK();
	TestArabic();

	std::cout << "============================" << std::endl;
	std::cout << "PASS: " << g_pass
		<< "  FAIL: " << g_fail
		<< "  TOTAL: " << (g_pass + g_fail) << std::endl;
	if (g_fail == 0)
		std::cout << "All tests passed!" << std::endl;
	std::cout << std::endl;
}
