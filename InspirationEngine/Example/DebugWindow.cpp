#include "../InspirationEngine/InspirationEngine.h"
#include "DebugWindow.h"

void cDebugWindow::draw()
{
	int aTile[32 * 64];
	memset(aTile, 0xcc, sizeof(int) * 32 * 64);
	drawBuffer(aTile, 0, 32, 64, 100, 100);
}

void cDebugWindow::callXButton()
{
	hideWindow();
}