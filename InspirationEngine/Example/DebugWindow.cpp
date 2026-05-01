#include "../InspirationEngine/InspirationEngine.h"
#include "DebugWindow.h"

void DebugWindow::draw()
{
	m_DebugText.draw();
}

void DebugWindow::callXButton()
{
	hideWindow();
}

void DebugWindow::initWindow()
{
	//임시로 포커스 가있는 텍스트박스를 디버그용 텍스트박스에 줌
	IECore::m_focusedTextBox = &m_DebugText;

	Font* lpFont = IECore::m_Font.getFont(0);
	//폰트랑 렌더러 설정
	m_DebugText.setFont(lpFont);
	m_DebugText.setRenderer(getRenderer(0));
	m_DebugText.setRect(0, 0, getWidth(), getHeight());
	m_DebugText.setStyle(dTEXT_BOX_AUTO_SPACE_NEXTLINE);
	m_DebugText.setCusorPos(0);
	m_DebugText.setCursurScreenPos(0, 0);
}
