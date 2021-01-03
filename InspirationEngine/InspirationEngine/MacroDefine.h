#pragma once

//어디 한곳에 놓기 애매한 매크로

#define KILL(ptr) {if(ptr != nullptr) delete ptr; ptr = nullptr;}		//단일 삭제
#define pKILL(ptr) {if(ptr != nullptr) delete[] ptr; ptr = nullptr;}	//배열 삭제

/// <summary>
/// 정수형 X, Y있는 변수
/// </summary>
class cIVector2
{
public:
	int m_iX;
	int m_iY;

private:


public:

	cIVector2()
	{
		reset();
	}

	void reset()
	{
		m_iX = 0;
		m_iY = 0;
	}

private:


};