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
#include <functional>
#include "utf8proc.h"
#include "Util/IEStrUTF8.h"
#include "Util/IELocalize.h"
#include "Core/MacroDefine.h"	//매크로 / Define용 헤더
#include "Text/IEFont.h"			//폰트 관리
#include "Text/IETextRenderer.h"//FreeType+HarfBuzz 렌더링 유틸리티
#include "Input/IEInput.h"		//입력 처리
#include "UI/IEUIBase.h"			//UI 위젯 공통 베이스
#include "UI/IEUILabel.h"				//UI-레이블
#include "UI/IEUITextBox.h"			//UI-텍스트박스
#include "UI/IEUIButton.h"			//UI-버튼
#include "UI/IEUIScrollView.h"		//UI-스크롤뷰
#include "UI/IETreeNode.h"			//UI-트리노드
#include "UI/IEUITreeView.h"			//UI-트리뷰
#include "UI/IEUISlider.h"			//UI-슬라이더
#include "UI/IEUIListView.h"			//UI-리스트뷰
#include "UI/IEUIDropdown.h"			//UI-드롭다운
#include "UI/IEPanelLayout.h"		//UI-패널레이아웃
#include "UI/IEUISection.h"			//UI-섹션 컨테이너
#include "UI/IEUIHRow.h"			//UI-가로 다중 위젯 행
#include "UI/IEUIFileBrowser.h"		//UI-파일브라우저
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
#include "Camera/IECameraComponent.h"		//카메라 컴포넌트
#include "Camera/IECameraObject.h"			//카메라 오브젝트
#include "Scene/IEScene.h"				//씬 기반
#include "Scene/IESceneManager.h"		//씬 관리자
#include "Scene/IESceneSerializer.h"	//씬 직렬화
#include "Scene/IEProjectConfig.h"		//프로젝트 설정
#include "Util/IEFile.h"			//파일
#include "Core/IEDebugInfo.h"		//디버그 정보
#include "Core/IECore.h"		//엔진
