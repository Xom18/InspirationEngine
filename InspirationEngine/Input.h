#pragma once

class cInput	//�Է¹޴� Ŭ����, ������ȭ �ʿ�
{
public:
	void start();
	const Uint8* getKeyInput();
private:
	const Uint8* m_lpKeyInput;
};