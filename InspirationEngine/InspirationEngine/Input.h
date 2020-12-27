#pragma once

class cInput	//입력받는 클래스, 스레드화 필요
{
public:
	void start();
	const Uint8* getKeyInput();
private:
	const Uint8* m_lpKeyInput;
};