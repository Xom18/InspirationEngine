#pragma once

class DebugWindow : public IEWindow
{
public:
	TextBox m_DebugText;


private:



public:

	virtual void draw() override;
	virtual void callXButton() override;

	void initWindow();

private:



};
