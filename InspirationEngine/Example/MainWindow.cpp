#include <stdio.h>
#include <SDL/SDL_ttf.h>
#include "../InspirationEngine/InspirationEngine.h"
#include "MainWindow.h"

void MainWindow::update(float deltaTime)
{
	const Uint8* keys = SDL_GetKeyboardState(nullptr);

	static bool f1Prev = false;
	bool f1Now = keys[SDL_SCANCODE_F1] != 0;
	if (f1Now && !f1Prev)
		m_debug.m_visible = !m_debug.m_visible;
	f1Prev = f1Now;

	static bool f2Prev = false;
	bool f2Now = keys[SDL_SCANCODE_F2] != 0;
	if (f2Now && !f2Prev)
	{
		IERenderer* pRenderer = getRenderer(1);
		if (pRenderer)
			pRenderer->saveScreenshot("debug_shot.png");
	}
	f2Prev = f2Now;
}

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

	if (m_debug.m_visible)
		m_debug.drawOverlay(pRenderer, IECore::m_Scene.getCurrentScene(), IECore::m_Font.getFont(0));
}

void MainWindow::callXButton()
{
	IECore::stopEngine();
}
