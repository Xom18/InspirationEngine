#include <string>
#include <algorithm>
#include "StrUTF8.h"

void cStrUTF8::pop_back(std::string* _lpString)
{
	if(_lpString->length() == 0)
		return;

	const char* lpText = _lpString->c_str();
	int iLength = static_cast<int>(_lpString->length());

	if((lpText[iLength - 1] & 0b10000000) == 0)
	{//아스키쪽이면 단순히 마지막 제거
		_lpString->pop_back();
	}
	else
	{//UTF-8이다
		//몇개 지워야되는지 체크
		int iDeleteCount = 0;
		for(int i = iLength - 1; i >= 0; --i)
		{
			++iDeleteCount;
			if((lpText[i] & 0b11000000) == 0b11000000)
				break;
		}

		//지워야되는 만큼 삭제
		_lpString->erase(static_cast<size_t>(iLength) - iDeleteCount);
	}
}

void cStrUTF8::pop_front(std::string* _lpString)
{
	size_t iLength = _lpString->length();
	if(iLength == 0)
		return;

	const char* lpText = _lpString->c_str();

	if((lpText[0] & 0b10000000) == 0)
	{//아스키쪽이면 시작꺼 바로 제거
		_lpString->erase(0, 1);
	}
	else
	{//UTF-8이다
		//몇개 지워야되는지 체크
		size_t szDeleteCount = 1;
		for(size_t i = 1; i < iLength; ++i)
		{
			if((lpText[i] & 0b11000000) == 0b11000000)
				break;
			++szDeleteCount;
		}

		//지워야되는 만큼 삭제
		_lpString->erase(0, static_cast<size_t>(szDeleteCount));
	}
}

size_t cStrUTF8::getMemoryPoint(std::string* _lpString, size_t _szPoint)
{
	size_t szLength = _lpString->length();
	const char* lpText = _lpString->c_str();

	size_t szCharPoint = 0;
	for(; szCharPoint < szLength && 0 < _szPoint; ++szCharPoint)
	{
		//단일 아스키문자
		if((lpText[szCharPoint] & 0b10000000) == 0)
		{
			--_szPoint;
		}
		//UTF-8 문자
		else if((lpText[szCharPoint] & 0b11000000) == 0b11000000)
		{
			--_szPoint;

			//UTF-8 끝까지 달린다
			while(szCharPoint < szLength)
			{
				++szCharPoint;
				if((lpText[szCharPoint + 1] & 0b11000000) == 0b11000000
				|| (lpText[szCharPoint + 1] & 0b10000000) == 0)
					break;
			}
		}
	}

	//길이초과했던거네
	if(szCharPoint == szLength)
		return std::string::npos;

	return szCharPoint;
}

void cStrUTF8::removeToFront(std::string* _lpString, size_t _szPoint, size_t _szCount)
{
	//제일 앞에있어서 지울게 없다
	if(_szPoint == 0)
		return;

	size_t iLength = _lpString->length();
	if(iLength == 0)
		return;

	const char* lpText = _lpString->c_str();

	//이 앞에문자를 삭제할꺼기때문에 -1빼고 시작
	size_t i = _szPoint - 1;
	size_t szDelCounter = _szCount;
	for(; i > 0; --i)
	{
		//단일 아스키문자
		if((lpText[i] & 0b10000000) == 0)
			--szDelCounter;
		//UTF-8 문자
		else if((lpText[i] & 0b11000000) == 0b11000000)
			--szDelCounter;

		//처리가 끝났소
		if(szDelCounter <= 0)
			break;
	}

	size_t szEreaseRange = _szPoint - i;
	_lpString->erase(i, szEreaseRange);
}

void cStrUTF8::removeToBack(std::string* _lpString, size_t _szPoint, size_t _szCount)
{
	size_t iLength = _lpString->length();
	if(iLength == 0)
		return;

	const char* lpText = _lpString->c_str();

	size_t i = _szPoint;
	size_t szDelCounter = _szCount;
	for(; i < iLength && szDelCounter > 0; ++i)
	{
		//단일 아스키문자
		if((lpText[i] & 0b10000000) == 0)
			--szDelCounter;
		//UTF-8 문자
		else if((lpText[i] & 0b11000000) == 0b11000000)
		{
			--szDelCounter;
			//UTF-8 끝까지 달린다
			while(i < iLength)
			{
				++i;
				if((lpText[i + 1] & 0b11000000) == 0b11000000
				|| (lpText[i + 1] & 0b10000000) == 0)
					break;
			}
		}
	}

	size_t iEreaseRange = i - _szPoint;
	_lpString->erase(_szPoint, iEreaseRange);
}