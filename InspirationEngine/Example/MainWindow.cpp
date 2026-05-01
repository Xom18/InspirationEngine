#include <stdio.h>
#include <SDL/SDL_ttf.h>
#include "../InspirationEngine/InspirationEngine.h"
#include "MainWindow.h"

void MainWindow::draw()
{
	IERenderer* pRenderer = getRenderer(1);
	if (pRenderer == nullptr)
		return;

	static SDL_Surface* Surface;
	static bool isOn = false;
	if (isOn == false)
	{
		Surface = IMG_Load("../data/DungeonCrawl_ProjectUtumnoTileset.png");
		isOn = true;
	}

	pRenderer->drawSurface(Surface, 0, 0);
}

void MainWindow::callXButton()
{
	IECore::stopEngine();
}
