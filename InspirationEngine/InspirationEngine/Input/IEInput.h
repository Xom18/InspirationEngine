#pragma once

/// <summary>
/// 모든 입력관련 이벤트 처리하는곳
/// </summary>
class IEInput
{
public:
	IEInput() {}
	~IEInput() {}

	/// <summary>
	/// 키 입력 상태 설정
	/// </summary>
	/// <param name="scanCode">대상 키 스캔코드</param>
	/// <param name="down">눌림: true, 뗌: false</param>
	void SetKeyState(SDL_Scancode scanCode, bool down)
	{
		m_keyInput[scanCode] = down;
	}

	/// <summary>
	/// 키 입력 상태 반환 — 눌려있으면 true
	/// </summary>
	/// <param name="scanCode">확인할 키 스캔코드</param>
	bool GetKeyState(SDL_Scancode scanCode)
	{
		return m_keyInput[scanCode];
	}

	/// <summary>
	/// 마우스 위치 갱신 및 이동량 계산
	/// </summary>
	/// <param name="x">마우스 X 좌표</param>
	/// <param name="y">마우스 Y 좌표</param>
	void UpdateMousePos(int32_t x, int32_t y)
	{
		m_mouseDelta.SetX(x - m_mousePos.GetX());
		m_mouseDelta.SetY(y - m_mousePos.GetY());
		m_mousePos.SetX(x);
		m_mousePos.SetY(y);
	}

	/// <summary>
	/// 마우스 현재 위치 반환
	/// </summary>
	IEVector2 GetMousePos()
	{
		return m_mousePos;
	}

	/// <summary>
	/// 마우스 이동량 반환 (직전 프레임 대비 델타)
	/// </summary>
	IEVector2 GetMouseDelta()
	{
		return m_mouseDelta;
	}

private:
	bool      m_keyInput[SDL_SCANCODE_COUNT] = { false, };
	bool      m_isTextEditting               = false;
	std::mutex m_operateEventMutex;
	Uint32    m_mouseStatus                  = 0;
	IEVector2 m_mousePos;
	IEVector2 m_mouseDelta;
};
