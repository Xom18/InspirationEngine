#pragma once

class DebugWindow : public IEWindow
{
public:
	virtual void Draw() override;
	virtual void CallXButton() override;

	void InitWindow();

private:
	IETextBox m_DebugText;
};
