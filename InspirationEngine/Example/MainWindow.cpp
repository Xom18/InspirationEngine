#include "../InspirationEngine/InspirationEngine.h"
#include "MainWindow.h"

#include <SDL/SDL_ttf.h>
#include <stdio.h>

void cMainWindow::draw()
{

}

void cMainWindow::callXButton()
{
	cIECore::stopEngine();
}