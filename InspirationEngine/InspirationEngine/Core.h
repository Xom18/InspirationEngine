#pragma once

class InspirationEngine
{
public:
	cInput		m_Input;		//입력
	cDisplay	m_Display;		//게임 화면
	cDebugTool	m_DebugTool;	//디버그 툴
	SDL_Event	m_Event;	//이벤트처리
};