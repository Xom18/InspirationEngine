#include "../InspirationEngine/InspirationEngine.h"
#include "DebugWindow.h"

void cDebugWindow::draw()
{
	//텍스트 출력 테스트
	static bool onceCode = false;
	static TTF_Font* Fontt = nullptr;
	if(onceCode == false)
	{
		if(TTF_Init() != 0)
			return;

		Fontt = TTF_OpenFont("../data/arial.ttf", 20);
		onceCode = true;
	}

	SDL_Color color;
	memset(&color, 0xffffffff, sizeof(int));
	drawText(Fontt, m_lpEngine->strInputtingText.c_str(), color, 0, 0, 0);
}

void cDebugWindow::callXButton()
{
	hideWindow();
}