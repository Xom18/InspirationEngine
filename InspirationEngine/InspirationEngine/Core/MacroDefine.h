#pragma once

//어디 한곳에 놓기 애매한 매크로

#define KILL(ptr) {if(ptr != nullptr) delete ptr; ptr = nullptr;}		//단일 삭제
#define pKILL(ptr) {if(ptr != nullptr) delete[] ptr; ptr = nullptr;}	//배열 삭제

/// <summary>
/// 정수형 X, Y있는 변수
/// </summary>
class IEVector2
{
public:
	int32_t m_x;
	int32_t m_y;

private:


public:

	IEVector2()
	{
		reset();
	}

	void reset()
	{
		m_x = 0;
		m_y = 0;
	}

private:


};