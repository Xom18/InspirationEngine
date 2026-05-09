#pragma once

#define dCHAR_TYPE_ASCII 0	//아스키
#define dCHAR_TYPE_UTF8_B 1	//UTF8 시작부분
#define dCHAR_TYPE_UTF8_M 2	//UTF8 중간부분

class IEStrUTF8
{
public:

	/// <summary>
	/// UTF-8 유효 시작 바이트인지 확인
	/// </summary>
	/// <param name="c">확인할 바이트</param>
	static bool IsValidStartByte(unsigned char c);

	/// <summary>
	/// UTF-8 시작 바이트로부터 시퀀스 바이트 수 반환
	/// </summary>
	/// <param name="c">UTF-8 시작 바이트</param>
	static size_t GetUTF8ByteLength(unsigned char c);

	/// <summary>
	/// 제일 뒤 문자 하나 삭제
	/// </summary>
	/// <param name="str">대상 문자열</param>
	static void pop_back(std::string& str);

	/// <summary>
	/// 제일 앞 문자 하나 삭제
	/// </summary>
	/// <param name="str">대상 문자열</param>
	static void pop_front(std::string& str);

	/// <summary>
	/// point로 부터 앞에있는 문자 count개 삭제
	/// </summary>
	/// <param name="str">대상 문자열</param>
	/// <param name="point">제거 시작 위치</param>
	/// <param name="count">지울 개수</param>
	static void removeToFront(std::string& str, size_t point, size_t count = 1);

	/// <summary>
	/// point로 부터 뒤에있는 문자 count개 삭제
	/// </summary>
	/// <param name="str">대상 문자열</param>
	/// <param name="point">제거 시작 위치</param>
	/// <param name="count">지울 개수</param>
	static void removeToBack(std::string& str, size_t point, size_t count = 1);

	/// <summary>
	/// 메모리상에서의 문자 위치 받아오기
	/// </summary>
	/// <param name="str">대상 문자열</param>
	/// <param name="point">UTF-8에서의 문자 위치</param>
	static size_t getMemoryPoint(std::string& str, size_t point);

	/// <summary>
	/// 해당 문자가 ascii인지 utf-8앞인지 utf-8 중간인지 확인해줌
	/// </summary>
	/// <param name="c">확인할 문자</param>
	/// <returns>dCHAR_TYPE_ASCII / dCHAR_TYPE_UTF8_B / dCHAR_TYPE_UTF8_M</returns>
	static int32_t isCharType(char _cChar)
	{
		if ((_cChar & 0b10000000) == 0)
			return dCHAR_TYPE_ASCII;
		if ((_cChar & 0b11000000) == 0b11000000)
			return dCHAR_TYPE_UTF8_B;
		return dCHAR_TYPE_UTF8_M;
	}
};
