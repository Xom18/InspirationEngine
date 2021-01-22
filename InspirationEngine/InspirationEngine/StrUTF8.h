#pragma once

#define dCHAR_TYPE_ASCII 0	//아스키
#define dCHAR_TYPE_UTF8_B 1	//UTF8 시작부분
#define dCHAR_TYPE_UTF8_M 2	//UTF8 중간부분

class cStrUTF8
{
public:

	/// <summary>
	/// 제일 뒤 문자 하나 삭제
	/// </summary>
	/// <param name="_lpString">대상 문자열</param>
	static void pop_back(std::string* _lpString);

	/// <summary>
	/// 제일 앞 문자 하나 삭제
	/// </summary>
	/// <param name="_lpString">대상 문자열</param>
	static void pop_front(std::string* _lpString);

	/// <summary>
	/// _iPoint로 부터 앞에있는 문자 _iCount개 삭제
	/// </summary>
	/// <param name="_lpString">대상 문자열</param>
	/// <param name="_iPoint">제거 시작 위치</param>
	/// <param name="_iCount">지울 개수</param>
	/// <param name="_bIsMemPoint">_iPoint가 메모리상에서의 위치인지</param>
	static void removeToFront(std::string* _lpString, size_t _szPoint, size_t _szCount = 1);

	/// <summary>
	/// _iPoint로 부터 뒤에있는 문자 _iCount개 삭제
	/// </summary>
	/// <param name="_lpString">대상 문자열</param>
	/// <param name="_iPoint">제거 시작 위치</param>
	/// <param name="_iCount">지울 개수</param>
	/// <param name="_bIsMemPoint">_iPoint가 메모리상에서의 위치인지</param>
	static void removeToBack(std::string* _lpString, size_t _szPoint, size_t _szCount = 1);

	/// <summary>
	/// 메모리상에서의 문자 위치 받아오기
	/// </summary>
	/// <param name="_lpString">대상 문자열</param>
	/// <param name="_iPoint">UTF-8에서의 문자 위치</param>
	static size_t getMemoryPoint(std::string* _lpString, size_t _szPoint);

	/// <summary>
	/// 해당 문자가 ascii인지 utf-8앞인지 utf-8 중간인지 확인해줌
	/// </summary>
	/// <param name="">확인 할 문자</param>
	/// <returns>dCHAR_TYPE_ASCII / dCHAR_TYPE_UTF8_B / dCHAR_TYPE_UTF8_M</returns>
	static int isCharType(char _cChar)
	{
		if((_cChar & 0b10000000) == 0)
			return dCHAR_TYPE_ASCII;
		if((_cChar & 0b11000000) == 0b11000000)
			return dCHAR_TYPE_UTF8_B;
		return dCHAR_TYPE_UTF8_M;
	}
};