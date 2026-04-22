#include <iostream>
#include <string>
#include <cstdio>
#include <cassert>
#include "../InspirationEngine/utf8proc/utf8proc.h"
#include "../InspirationEngine/StrUTF8.h"

// 테스트용 헬퍼 함수
std::string BytesToHex(const std::string& str) {
	std::string hex;
	for (unsigned char c : str) {
		char buffer[3];
		sprintf_s(buffer, sizeof(buffer), "%02X", c);
		hex += buffer;
	}
	return hex;
}

// 테스트 케이스 1: 잘못된 연속 바이트 단독 (0x80 ~ 0xBF)
void TestInvalidContinuationByte() {
	std::cout << "=== Test 1: Invalid Continuation Byte ===" << std::endl;

	// Case 1: 연속 바이트 단독
	std::string invalid1 = "Hello\x80World";
	std::cout << "Before: " << BytesToHex(invalid1) << std::endl;
	std::cout << "Content: " << invalid1 << std::endl;

	cStrUTF8::pop_back(invalid1);
	std::cout << "After pop_back: " << BytesToHex(invalid1) << std::endl;
	std::cout << std::endl;

	// Case 2: 연속 바이트 단독 at front
	std::string invalid2 = "\x80Hello";
	std::cout << "Before: " << BytesToHex(invalid2) << std::endl;
	cStrUTF8::pop_front(invalid2);
	std::cout << "After pop_front: " << BytesToHex(invalid2) << std::endl;
	std::cout << std::endl;
}

// 테스트 케이스 2: 불완전한 UTF-8 시퀀스
void TestIncompleteMutliByte() {
	std::cout << "=== Test 2: Incomplete Multi-byte ===" << std::endl;

	// 2-byte UTF-8이 1바이트만 있는 경우 (0xC0)
	std::string incomplete1 = "Hello\xC0";
	std::cout << "Before: " << BytesToHex(incomplete1) << std::endl;
	std::cout << "getMemoryPoint(1): " << cStrUTF8::getMemoryPoint(incomplete1, 1) << std::endl;
	std::cout << std::endl;

	// 3-byte UTF-8이 불완전한 경우 (0xE0 0x80)
	std::string incomplete2 = "Hi\xE0\x80";
	std::cout << "Before: " << BytesToHex(incomplete2) << std::endl;
	std::cout << "getMemoryPoint(1): " << cStrUTF8::getMemoryPoint(incomplete2, 1) << std::endl;
	std::cout << std::endl;
}

// 테스트 케이스 3: 잘못된 시작 바이트
void TestInvalidStartByte() {
	std::cout << "=== Test 3: Invalid Start Byte ===" << std::endl;

	// 0xF5 이상은 UTF-8에서 유효하지 않음
	std::string invalid = "Test\xF5\x80\x80\x80" "End";
	std::cout << "Before: " << BytesToHex(invalid) << std::endl;
	std::cout << "getMemoryPoint(2): " << cStrUTF8::getMemoryPoint(invalid, 2) << std::endl;
	std::cout << std::endl;
}

// 테스트 케이스 4: 정상 UTF-8
void TestValidUTF8() {
	std::cout << "=== Test 4: Valid UTF-8 ===" << std::endl;

	std::string valid = "Hello가나다";
	std::cout << "Before: " << valid << " (" << BytesToHex(valid) << ")" << std::endl;

	// "가" = EAB080, "나" = EB82B0, "다" = EB8BA0
	size_t pos1 = cStrUTF8::getMemoryPoint(valid, 6);
	std::cout << "getMemoryPoint(6): " << pos1 << " (expected: 8 for first Korean char)" << std::endl;

	std::string test = valid;
	cStrUTF8::pop_back(test);
	std::cout << "After pop_back: " << test << std::endl;
	std::cout << std::endl;
}

// 테스트 케이스 5: removeToFront/removeToBack with invalid bytes
void TestRemoveWithInvalid() {
	std::cout << "=== Test 5: Remove with Invalid Bytes ===" << std::endl;

	// 정상 문자 앞에 잘못된 바이트
	std::string str1 = "\x80Hello";
	std::cout << "Before removeToFront: " << BytesToHex(str1) << std::endl;
	cStrUTF8::removeToFront(str1, 2, 1);
	std::cout << "After removeToFront(2, 1): " << BytesToHex(str1) << std::endl;
	std::cout << std::endl;

	// removeToBack with invalid
	std::string str2 = "Hello\x80";
	std::cout << "Before removeToBack: " << BytesToHex(str2) << std::endl;
	cStrUTF8::removeToBack(str2, 5, 1);
	std::cout << "After removeToBack(5, 1): " << BytesToHex(str2) << std::endl;
	std::cout << std::endl;
}

// 테스트 케이스 6: pop_back/pop_front with invalid bytes
void TestPopWithInvalid() {
	std::cout << "=== Test 6: Pop with Invalid Bytes ===" << std::endl;

	// pop_back: 문자열 끝이 연속 바이트일 때
	std::string str1 = "Test\x80";
	std::cout << "Before pop_back: " << BytesToHex(str1) << std::endl;
	cStrUTF8::pop_back(str1);
	std::cout << "After pop_back: " << BytesToHex(str1) << std::endl;
	std::cout << std::endl;

	// pop_front: 문자열 시작이 연속 바이트일 때
	std::string str2 = "\x80Test";
	std::cout << "Before pop_front: " << BytesToHex(str2) << std::endl;
	cStrUTF8::pop_front(str2);
	std::cout << "After pop_front: " << BytesToHex(str2) << std::endl;
	std::cout << std::endl;
}

// 테스트 케이스 7: Overlong encoding (0xC0, 0xC1)
void TestOverlongEncoding() {
	std::cout << "=== Test 7: Overlong Encoding ===" << std::endl;

	// 0xC0 0x80 is overlong encoding for U+0000
	// utf8proc_iterate는 이를 오류로 처리 → 1바이트 취급
	std::string overlong = "Test\xC0\x80" "End";
	std::cout << "Before: " << BytesToHex(overlong) << std::endl;
	std::cout << "getMemoryPoint(1): " << cStrUTF8::getMemoryPoint(overlong, 1) << std::endl;
	std::cout << std::endl;
}

// 테스트 케이스 8: 연속된 잘못된 바이트들
void TestConsecutiveInvalidBytes() {
	std::cout << "=== Test 8: Consecutive Invalid Bytes ===" << std::endl;

	std::string invalid = "Start\x80\x81\x82" "End";
	std::cout << "Before: " << BytesToHex(invalid) << std::endl;
	std::cout << "getMemoryPoint(2): " << cStrUTF8::getMemoryPoint(invalid, 2) << std::endl;

	std::string test = invalid;
	cStrUTF8::removeToBack(test, 5, 3);
	std::cout << "After removeToBack(5, 3): " << BytesToHex(test) << std::endl;
	std::cout << std::endl;
}

void TestStrUTF8() {
	std::cout << "UTF-8 Invalid Input Test Suite\n" << std::endl;

	TestInvalidContinuationByte();
	TestIncompleteMutliByte();
	TestInvalidStartByte();
	TestValidUTF8();
	TestRemoveWithInvalid();
	TestPopWithInvalid();
	TestOverlongEncoding();
	TestConsecutiveInvalidBytes();

	std::cout << "All tests completed!" << std::endl;
}
