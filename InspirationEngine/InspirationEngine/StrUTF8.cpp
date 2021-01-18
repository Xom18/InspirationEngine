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
		int iDeleteCount = 1;
		for(int i = 1; i < iLength; ++i)
		{
			if((lpText[i] & 0b11000000) == 0b11000000)
				break;
			++iDeleteCount;
		}

		//지워야되는 만큼 삭제
		_lpString->erase(0, static_cast<size_t>(iDeleteCount));
	}
}

int cStrUTF8::getMemoryPoint(std::string* _lpString, int _iPoint)
{
	size_t iLength = _lpString->length();
	const char* lpText = _lpString->c_str();

	int iCharPoint = 0;
	for(; iCharPoint < iLength && 0 < _iPoint; ++iCharPoint)
	{
		//단일 아스키문자
		if((lpText[iCharPoint] & 0b10000000) == 0)
		{
			--_iPoint;
		}
		//UTF-8 문자
		else if((lpText[iCharPoint] & 0b11000000) == 0b11000000)
		{
			--_iPoint;

			//UTF-8 끝까지 달린다
			while(iCharPoint < iLength)
			{
				++iCharPoint;
				if((lpText[iCharPoint + 1] & 0b11000000) == 0b11000000
				|| (lpText[iCharPoint + 1] & 0b10000000) == 0)
					break;
			}
		}
	}


	return iCharPoint;
}

void cStrUTF8::removeToFront(std::string* _lpString, int _iPoint, int _iCount)
{
	size_t iLength = _lpString->length();
	if(iLength == 0)
		return;

	const char* lpText = _lpString->c_str();

	//이 앞에문자를 삭제할꺼기때문에 -1빼고 시작
	int i = _iPoint - 1;
	int iDelCounter = _iCount;
	for(; i >= 0; --i)
	{
		//단일 아스키문자
		if((lpText[i] & 0b10000000) == 0)
			--iDelCounter;
		//UTF-8 문자
		else if((lpText[i] & 0b11000000) == 0b11000000)
			--iDelCounter;

		//처리가 끝났소
		if(iDelCounter <= 0)
			break;
	}
	i = std::max(0, i);

	int iEreaseRange = _iPoint - i;
	_lpString->erase(i, iEreaseRange);
}

void cStrUTF8::removeToBack(std::string* _lpString, int _iPoint, int _iCount)
{
	size_t iLength = _lpString->length();
	if(iLength == 0)
		return;

	const char* lpText = _lpString->c_str();

	int i = _iPoint;
	int iDelCounter = _iCount;
	for(; i < iLength && iDelCounter > 0; ++i)
	{
		//단일 아스키문자
		if((lpText[i] & 0b10000000) == 0)
			--iDelCounter;
		//UTF-8 문자
		else if((lpText[i] & 0b11000000) == 0b11000000)
		{
			--iDelCounter;
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

	int iEreaseRange = i - _iPoint;
	_lpString->erase(_iPoint, iEreaseRange);
}