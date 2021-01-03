#include "../InspirationEngine/InspirationEngine.h"
#include "MainWindow.h"

void cMainWindow::draw()
{
	int aTile[64 * 64];
	memset(aTile, 0xff, sizeof(int) * 64 * 64);
	for(int i = 0; i < 64 * 64; ++i)
	{
		aTile[i] = ((i * 3) << 4) + 0xff0000ff;
	}
	drawBuffer(aTile, 0, 64, 64, 0, 0, SDL_BLENDMODE_BLEND, 100, 100);
}

void cMainWindow::callXButton()
{
	m_lpEngine->stopEngine();
}