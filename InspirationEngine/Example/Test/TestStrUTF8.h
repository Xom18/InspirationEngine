#pragma once
#include "Test.h"

class TestStrUTF8 : public Test
{
public:
	TestStrUTF8() { m_name = "IEStrUTF8"; }
	void run() override;

private:
	void testInvalidContinuationByte();
	void testIncompleteMutliByte();
	void testInvalidStartByte();
	void testValidUTF8();
	void testRemoveWithInvalid();
	void testPopWithInvalid();
	void testOverlongEncoding();
	void testConsecutiveInvalidBytes();
	void testCJK();
	void testArabic();
};
