#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
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
#include "utf8proc.h"
#include "Util/StrUTF8.h"
#include "Core/MacroDefine.h"	//매크로 / Define용 헤더
#include "Text/Font.h"			//폰트 관리
#include "Text/IETextRenderer.h"//FreeType+HarfBuzz 렌더링 유틸리티
#include "Input/IEInput.h"		//입력 처리
#include "UI/Menu.h"			//메뉴
#include "UI/TextBox.h"			//메뉴-텍스트박스
#include "Input/MouseCusor.h"	//마우스 커서
#include "Window/IERenderer.h"	//렌더러
#include "Window/IEWindow.h"	//창
#include "Util/File.h"			//파일
#include "Core/DebugInfo.h"		//디버그 정보
#include "Core/IECore.h"		//엔진
