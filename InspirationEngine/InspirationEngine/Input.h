#pragma once

/// <summary>
/// 모든 입력관련 이벤트 처리하는곳
/// </summary>
class cInput
{
public:

private:
	/// <summary>
	/// 안눌려있으면 false
	/// 눌려있으면 true
	/// </summary>
	bool m_aKeyInput[SDL_NUM_SCANCODES];
	std::mutex m_mtxOperateEvent;

public:
	cInput(){};
	~cInput(){};

	void setKeyState(SDL_Scancode _ScanCode, Uint8 _State)
	{
		m_aKeyInput[_ScanCode] = _State;
	}

	bool getKeyState(SDL_Scancode _ScanCode)
	{
		return m_aKeyInput[_ScanCode];
	}

private:

};