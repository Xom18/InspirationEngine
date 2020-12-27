#include <SDL/SDL.h>
#include "MacroDefine.h"
#include "Input.h"

void cInput::start()
{
	m_lpKeyInput = SDL_GetKeyboardState(NULL);
}

const Uint8* cInput::getKeyInput()
{
	return m_lpKeyInput;
}