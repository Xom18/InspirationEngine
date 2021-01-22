#include "../InspirationEngine/InspirationEngine.h"
#include "DebugWindow.h"

void cDebugWindow::draw()
{
	//텍스트 출력 테스트
	SDL_Color color;
	memset(&color, 0xffffffff, sizeof(int));
	cRenderer* lpRenderer = getRenderer(0);

	m_DebugText.draw();
}

void cDebugWindow::callXButton()
{
	hideWindow();
}

void cDebugWindow::initWindow()
{
	//텍스트 입력 테스트용
	SDL_StartTextInput();

	//임시로 포커스 가있는 텍스트박스를 디버그용 텍스트박스에 줌
	cIECore::m_lpFocusedTextBox = &m_DebugText;

	//폰트랑 렌더러 설정
	m_DebugText.setFont(cIECore::m_Font.getFont(0));
	m_DebugText.setRenderer(getRenderer(0));
	m_DebugText.setRect(0, 0, getWidth(), getHeight());
	m_DebugText.setStyle(dTEXT_BOX_AUTO_SPACE_NEXTLINE);
	m_DebugText.setCusorPos(0);
}