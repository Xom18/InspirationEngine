#pragma once

class DebugWindow : public IEWindow
{
public:
	virtual void draw() override;
	virtual void callXButton() override;

	void initWindow();

private:
	IETextBox m_DebugText;
};
