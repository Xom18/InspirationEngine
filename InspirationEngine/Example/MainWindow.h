#pragma once

class MainWindow : public IEWindow
{
public:
	virtual void update(float deltaTime) override;
	virtual void draw() override;
	virtual void callXButton() override;

private:
	IEDebugInfo m_debug;
};
