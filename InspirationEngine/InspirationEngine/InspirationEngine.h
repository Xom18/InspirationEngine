#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
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
#include "Util/IEStrUTF8.h"
#include "Core/MacroDefine.h"	//매크로 / Define용 헤더
#include "Text/IEFont.h"			//폰트 관리
#include "Text/IETextRenderer.h"//FreeType+HarfBuzz 렌더링 유틸리티
#include "Input/IEInput.h"		//입력 처리
#include "UI/IEUIBase.h"			//UI 위젯 공통 베이스
#include "UI/IELabel.h"				//UI-레이블
#include "UI/IETextBox.h"			//UI-텍스트박스
#include "UI/IEButton.h"			//UI-버튼
#include "UI/IEScrollView.h"		//UI-스크롤뷰
#include "UI/IETreeNode.h"			//UI-트리노드
#include "UI/IETreeView.h"			//UI-트리뷰
#include "UI/IESlider.h"			//UI-슬라이더
#include "UI/IEPanelLayout.h"		//UI-패널레이아웃
#include "UI/IEFileBrowser.h"		//UI-파일브라우저
#include "Input/MouseCusor.h"	//마우스 커서
#include "Window/IERenderer.h"	//렌더러
#include "Window/IEWindow.h"	//창
#include "UI/IEPanel.h"				//UI-패널 기반
#include "UI/IEDockedPanel.h"		//UI-도킹패널
#include "UI/IEFloatingPanel.h"		//UI-플로팅패널
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
#include "Util/IEFile.h"			//파일
#include "Core/IEDebugInfo.h"		//디버그 정보
#include "Core/IECore.h"		//엔진
