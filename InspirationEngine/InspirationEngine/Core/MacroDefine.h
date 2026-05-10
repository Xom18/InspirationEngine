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
	IEVector2() { Reset(); }

	void Reset() { m_x = 0; m_y = 0; }

	int32_t GetX() const { return m_x; }
	int32_t GetY() const { return m_y; }
	void SetX(int32_t x) { m_x = x; }
	void SetY(int32_t y) { m_y = y; }

private:
	int32_t m_x = 0;
	int32_t m_y = 0;
};
