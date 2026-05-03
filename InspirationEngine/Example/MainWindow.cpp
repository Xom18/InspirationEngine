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
		m_debug.SetVisible(!m_debug.IsVisible());
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

	IEScene* pScene = IECore::m_Scene.GetCurrentScene();
	if (pScene != nullptr)
		pScene->Draw(pRenderer);

	if (m_debug.IsVisible())
		m_debug.DrawOverlay(pRenderer, pScene, IECore::m_Font.getFont(0));
}

void MainWindow::callXButton()
{
	IECore::stopEngine();
}
