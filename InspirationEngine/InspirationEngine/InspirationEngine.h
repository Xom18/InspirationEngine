#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <algorithm>
#include <thread>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
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
#include "Sprite/IESpriteManager.h"	//스프라이트 관리
#include "Sprite/IEAtlasManager.h"	//타일 아틀라스 관리
#include "GameObject/IEGameObject.h"	//게임 오브젝트 기반
#include "GameObject/IEStaticObject.h"	//정적 오브젝트
#include "GameObject/IEEntity.h"		//동적 오브젝트
#include "Camera/IECamera.h"					//카메라 기반
#include "Camera/IECameraTopView.h"			//탑뷰 카메라
#include "Camera/IECameraIsometric.h"			//아이소메트릭 카메라
#include "Camera/IECameraSideView.h"			//사이드뷰 카메라
#include "Camera/IECameraOverheadOblique.h"	//오버헤드 오블리크 카메라
#include "Camera/IECameraDepthSide.h"			//뎁스 사이드뷰 카메라
#include "Scene/IEScene.h"				//씬 기반
#include "Scene/IESceneManager.h"		//씬 관리자
#include "Util/File.h"			//파일
#include "Core/DebugInfo.h"		//디버그 정보
#include "Core/IECore.h"		//엔진
