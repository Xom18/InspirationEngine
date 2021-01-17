#pragma once

class cDebugWindow: public cWindow
{
public:
	cTextBox m_DebugText;


private:



public:

	virtual void draw() override;
	virtual void callXButton() override;

	void initWindow();

private:



};