#include "../InspirationEngine/InspirationEngine.h"
#include "DebugWindow.h"

void DebugWindow::Draw()
{
	m_DebugText.Draw();
}

void DebugWindow::CallXButton()
{
	HideWindow();
}

void DebugWindow::InitWindow()
{
	//임시로 포커스 가있는 텍스트박스를 디버그용 텍스트박스에 줌
	IECore::SetFocusedTextBox(&m_DebugText);

	IEFont* font = IECore::GetFont().GetFont(0);
	//폰트랑 렌더러 설정
	m_DebugText.SetFont(font);
	m_DebugText.SetRenderer(GetRenderer(0));
	m_DebugText.SetRect(0, 0, GetWidth(), GetHeight());
	m_DebugText.SetStyle(dTEXT_BOX_AUTO_SPACE_NEXTLINE);
	m_DebugText.SetCursorPos(0);
	m_DebugText.SetCursorScreenPos(0, 0);
}
