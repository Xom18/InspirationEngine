#include "TestStrUTF8.h"
#include "utf8proc.h"
#include "../../InspirationEngine/Util/IEStrUTF8.h"

void TestStrUTF8::run()
{
	std::cout << "===== UTF-8 Test Suite =====" << std::endl << std::endl;

	testInvalidContinuationByte();
	testIncompleteMutliByte();
	testInvalidStartByte();
	testValidUTF8();
	testRemoveWithInvalid();
	testPopWithInvalid();
	testOverlongEncoding();
	testConsecutiveInvalidBytes();
	testCJK();
	testArabic();

	printSummary();
}

void TestStrUTF8::testInvalidContinuationByte()
{
	printSection("Test 1: Invalid Continuation Byte");

	std::string s1 = "Hello\x80World";
	IEStrUTF8::pop_back(s1);
	checkStr("pop_back (invalid mid)", s1, "Hello\x80" "Worl");

	std::string s2 = "\x80Hello";
	IEStrUTF8::pop_front(s2);
	checkStr("pop_front (leading 0x80)", s2, "Hello");

	std::cout << std::endl;
}

void TestStrUTF8::testIncompleteMutliByte()
{
	printSection("Test 2: Incomplete Multi-byte");

	std::string s1 = "Hello\xC0";
	checkSize("getMemoryPoint(1) 'Hello\\xC0'", IEStrUTF8::getMemoryPoint(s1, 1), 1u);

	std::string s2 = "Hi\xE0\x80";
	checkSize("getMemoryPoint(1) 'Hi\\xE0\\x80'", IEStrUTF8::getMemoryPoint(s2, 1), 1u);

	std::cout << std::endl;
}

void TestStrUTF8::testInvalidStartByte()
{
	printSection("Test 3: Invalid Start Byte");

	std::string s = "Test\xF5\x80\x80\x80" "End";
	checkSize("getMemoryPoint(2) 'Test\\xF5...'", IEStrUTF8::getMemoryPoint(s, 2), 2u);

	std::cout << std::endl;
}

void TestStrUTF8::testValidUTF8()
{
	printSection("Test 4: Valid UTF-8");

	// "Hello가나다": H(0)e(1)l(2)l(3)o(4)가(5)나(8)다(11)
	std::string s = "Hello\xEA\xB0\x80\xEB\x82\xB0\xEB\x8B\xA0";
	checkSize("getMemoryPoint(6) → byte 8 (나)", IEStrUTF8::getMemoryPoint(s, 6), 8u);

	std::string t = s;
	IEStrUTF8::pop_back(t);
	checkStr("pop_back removes 다", t, "Hello\xEA\xB0\x80\xEB\x82\xB0");

	std::cout << std::endl;
}

void TestStrUTF8::testRemoveWithInvalid()
{
	printSection("Test 5: Remove with Invalid Bytes");

	std::string s1 = "\x80Hello";
	IEStrUTF8::removeToFront(s1, 2, 1);
	checkStr("removeToFront(2,1) '\\x80Hello'", s1, "\x80" "ello");

	std::string s2 = "Hello\x80";
	IEStrUTF8::removeToBack(s2, 5, 1);
	checkStr("removeToBack(5,1) 'Hello\\x80'", s2, "Hello");

	std::cout << std::endl;
}

void TestStrUTF8::testPopWithInvalid()
{
	printSection("Test 6: Pop with Invalid Bytes");

	std::string s1 = "Test\x80";
	IEStrUTF8::pop_back(s1);
	checkStr("pop_back 'Test\\x80'", s1, "Test");

	std::string s2 = "\x80Test";
	IEStrUTF8::pop_front(s2);
	checkStr("pop_front '\\x80Test'", s2, "Test");

	std::cout << std::endl;
}

void TestStrUTF8::testOverlongEncoding()
{
	printSection("Test 7: Overlong Encoding");

	std::string s = "Test\xC0\x80" "End";
	checkSize("getMemoryPoint(1) 'Test\\xC0\\x80End'", IEStrUTF8::getMemoryPoint(s, 1), 1u);

	std::cout << std::endl;
}

void TestStrUTF8::testConsecutiveInvalidBytes()
{
	printSection("Test 8: Consecutive Invalid Bytes");

	std::string s = "Start\x80\x81\x82" "End";
	checkSize("getMemoryPoint(2) 'Start\\x80\\x81\\x82End'", IEStrUTF8::getMemoryPoint(s, 2), 2u);

	std::string t = s;
	IEStrUTF8::removeToBack(t, 5, 3);
	checkStr("removeToBack(5,3) removes 3 invalid bytes", t, "StartEnd");

	std::cout << std::endl;
}

void TestStrUTF8::testCJK()
{
	printSection("Test 9: CJK Characters");

	// 你好世界: 각 3B, 총 12B
	std::string chinese =
		"\xE4\xBD\xA0"   // 你
		"\xE5\xA5\xBD"   // 好
		"\xE4\xB8\x96"   // 世
		"\xE7\x95\x8C";  // 界

	checkSize("getMemoryPoint(2) 你好→ byte 6", IEStrUTF8::getMemoryPoint(chinese, 2), 6u);

	std::string t = chinese;
	IEStrUTF8::pop_back(t);
	checkStr("pop_back removes 界(3B)", t, "\xE4\xBD\xA0\xE5\xA5\xBD\xE4\xB8\x96");

	t = chinese;
	IEStrUTF8::pop_front(t);
	checkStr("pop_front removes 你(3B)", t, "\xE5\xA5\xBD\xE4\xB8\x96\xE7\x95\x8C");

	t = chinese;
	IEStrUTF8::removeToFront(t, 6, 2);
	checkStr("removeToFront(6,2) removes 你好 → 世界", t, "\xE4\xB8\x96\xE7\x95\x8C");

	std::string japanese = "\xE6\x97\xA5\xE6\x9C\xAC\xE8\xAA\x9E";
	checkSize("getMemoryPoint(1) 日→ byte 3", IEStrUTF8::getMemoryPoint(japanese, 1), 3u);

	std::string mixed = "Hi\xE4\xBD\xA0\xE5\xA5\xBD";
	checkSize("getMemoryPoint(3) Hi你→ byte 5", IEStrUTF8::getMemoryPoint(mixed, 3), 5u);

	t = mixed;
	IEStrUTF8::pop_back(t);
	checkStr("pop_back removes 好(3B) → Hi你(5B)", t, "Hi\xE4\xBD\xA0");

	std::cout << std::endl;
}

void TestStrUTF8::testArabic()
{
	printSection("Test 10: Arabic Characters");

	// مرحبا: 각 2B, 총 10B
	std::string arabic =
		"\xD9\x85"   // م
		"\xD8\xB1"   // ر
		"\xD8\xAD"   // ح
		"\xD8\xA8"   // ب
		"\xD8\xA7";  // ا

	checkSize("getMemoryPoint(2) مر→ byte 4", IEStrUTF8::getMemoryPoint(arabic, 2), 4u);

	std::string t = arabic;
	IEStrUTF8::pop_back(t);
	checkStr("pop_back removes ا(2B)", t, "\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8");

	t = arabic;
	IEStrUTF8::pop_front(t);
	checkStr("pop_front removes م(2B)", t, "\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7");

	t = arabic;
	IEStrUTF8::removeToBack(t, 4, 2);
	checkStr("removeToBack(4,2) removes حب → مرا(6B)", t, "\xD9\x85\xD8\xB1\xD8\xA7");

	// مَر: م + fatha + ر → pop_back removes ر(2B)
	std::string withDiacritic = "\xD9\x85\xD9\x8E\xD8\xB1";
	t = withDiacritic;
	IEStrUTF8::pop_back(t);
	checkStr("pop_back on مَر removes ر → مَ(4B)", t, "\xD9\x85\xD9\x8E");

	std::cout << std::endl;
}
