#include <stdio.h>
#include "../InspirationEngine/InspirationEngine.h"
#include "MainWindow.h"

void MainWindow::Update(float deltaTime)
{
	const bool* keys = SDL_GetKeyboardState(nullptr);

	static bool f1Prev = false;
	bool f1Now = keys[SDL_SCANCODE_F1] != 0;
	if (f1Now && !f1Prev)
		m_debug.SetVisible(!m_debug.IsVisible());
	f1Prev = f1Now;

	static bool f2Prev = false;
	bool f2Now = keys[SDL_SCANCODE_F2] != 0;
	if (f2Now && !f2Prev)
	{
		IERenderer* pRenderer = GetRenderer(1);
		if (pRenderer != nullptr)
			pRenderer->SaveScreenshot("debug_shot.png");
	}
	f2Prev = f2Now;
}

void MainWindow::Draw()
{
	IERenderer* pRenderer = GetRenderer(1);
	if (pRenderer == nullptr)
		return;

	IEScene* pScene = IECore::GetScene().GetCurrentScene();
	if (pScene != nullptr)
		pScene->Draw(pRenderer);

	if (m_debug.IsVisible())
		m_debug.DrawOverlay(pRenderer, pScene, IECore::GetFont().GetFont(0));
}

void MainWindow::CallXButton()
{
	IECore::StopEngine();
}
