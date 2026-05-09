#pragma once

/// <summary>
/// 모든 입력관련 이벤트 처리하는곳
/// </summary>
class IEInput
{
public:

private:
	/// <summary>
	/// 안눌려있으면 false
	/// 눌려있으면 true
	/// </summary>
	bool m_keyInput[SDL_SCANCODE_COUNT] = { false, };
	bool m_isTextEditting = false;	//텍스트 입력중
	std::mutex m_operateEventMutex;

	Uint32 m_mouseStatus = 0;
	IEVector2 m_mousePos;
	IEVector2 m_mouseDelta;

public:
	IEInput() {}
	~IEInput() {}

	/// <summary>
	/// 키 입력 상태 설정
	/// </summary>
	/// <param name="scanCode">대상 키 스캔코드</param>
	/// <param name="state">눌림: 1, 뗌: 0</param>
	void setKeyState(SDL_Scancode scanCode, bool down)
	{
		m_keyInput[scanCode] = down;
	}

	/// <summary>
	/// 키 입력 상태 반환 — 눌려있으면 true
	/// </summary>
	/// <param name="scanCode">확인할 키 스캔코드</param>
	bool getKeyState(SDL_Scancode scanCode)
	{
		return m_keyInput[scanCode];
	}

	/// <summary>
	/// 마우스 위치 갱신 및 이동량 계산
	/// </summary>
	/// <param name="x">마우스 X 좌표</param>
	/// <param name="y">마우스 Y 좌표</param>
	void updateMousePos(int32_t x, int32_t y)
	{
		m_mouseDelta.m_x = x - m_mousePos.m_x;
		m_mouseDelta.m_y = y - m_mousePos.m_y;
		m_mousePos.m_x = x;
		m_mousePos.m_y = y;
	}

	/// <summary>
	/// 마우스 현재 위치 반환
	/// </summary>
	IEVector2 getMousePos()
	{
		return m_mousePos;
	}

	/// <summary>
	/// 마우스 이동량 반환 (직전 프레임 대비 델타)
	/// </summary>
	IEVector2 getMouseDelta()
	{
		return m_mouseDelta;
	}

private:

};
