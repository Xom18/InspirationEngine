#pragma once

class MainWindow : public IEWindow
{
public:
	virtual void Update(float deltaTime) override;
	virtual void Draw() override;
	virtual void CallXButton() override;

private:
	IEDebugInfo m_debug;
};
