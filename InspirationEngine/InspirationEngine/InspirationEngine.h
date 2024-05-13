#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_TTF.h>
#include <algorithm>
#include <thread>
#include <vector>
#include <string>
#include <map>
#include <deque>
#include <mutex>
#include <list>
#include <stack>
#include <sstream>
#include <optional>
#include "StrUTF8.h"
#include "MacroDefine.h"	//매크로 / Define용 헤더
#include "Font.h"			//폰트 관리
#include "Input.h"			//입력 처리
#include "Menu.h"			//메뉴
#include "TextBox.h"		//메뉴-텍스트박스
#include "MouseCusor.h"		//마우스 커서
#include "Renderer.h"		//렌더러
#include "Window.h"			//창
#include "File.h"			//파일
#include "DebugInfo.h"		//디버그 정보
#include "Core.h"			//엔진