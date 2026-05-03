#pragma once

class MainWindow : public IEWindow
{
public:
	DebugInfo m_debug;

public:
	virtual void update(float deltaTime) override;
	virtual void draw() override;
	virtual void callXButton() override;
};
