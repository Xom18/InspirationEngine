#pragma once

class IEStrUTF8
{
public:
    static constexpr int32_t kCharTypeAscii  = 0;
    static constexpr int32_t kCharTypeUTF8B  = 1;
    static constexpr int32_t kCharTypeUTF8M  = 2;

	/// <summary>
	/// UTF-8 유효 시작 바이트인지 확인
	/// </summary>
	/// <param name="c">확인할 바이트</param>
	static bool IsValidStartByte(uint8_t c);

	/// <summary>
	/// UTF-8 시작 바이트로부터 시퀀스 바이트 수 반환
	/// </summary>
	/// <param name="c">UTF-8 시작 바이트</param>
	static size_t GetUTF8ByteLength(uint8_t c);

	/// <summary>
	/// 제일 뒤 문자 하나 삭제
	/// </summary>
	/// <param name="str">대상 문자열</param>
	static void PopBack(std::string& str);

	/// <summary>
	/// 제일 앞 문자 하나 삭제
	/// </summary>
	/// <param name="str">대상 문자열</param>
	static void PopFront(std::string& str);

	/// <summary>
	/// point로 부터 앞에있는 문자 count개 삭제
	/// </summary>
	/// <param name="str">대상 문자열</param>
	/// <param name="point">제거 시작 위치</param>
	/// <param name="count">지울 개수</param>
	static void RemoveToFront(std::string& str, size_t point, size_t count = 1);

	/// <summary>
	/// point로 부터 뒤에있는 문자 count개 삭제
	/// </summary>
	/// <param name="str">대상 문자열</param>
	/// <param name="point">제거 시작 위치</param>
	/// <param name="count">지울 개수</param>
	static void RemoveToBack(std::string& str, size_t point, size_t count = 1);

	/// <summary>
	/// 메모리상에서의 문자 위치 받아오기
	/// </summary>
	/// <param name="str">대상 문자열</param>
	/// <param name="point">UTF-8에서의 문자 위치</param>
	static size_t GetMemoryPoint(std::string& str, size_t point);

	/// <summary>
	/// 해당 문자가 ASCII인지 UTF-8 시작 바이트인지 UTF-8 중간 바이트인지 확인
	/// </summary>
	/// <param name="c">확인할 문자</param>
	/// <returns>kCharTypeAscii / kCharTypeUTF8B / kCharTypeUTF8M</returns>
	static int32_t IsCharType(char c)
	{
		if ((c & 0b10000000) == 0)
			return kCharTypeAscii;
		if ((c & 0b11000000) == 0b11000000)
			return kCharTypeUTF8B;
		return kCharTypeUTF8M;
	}
};
