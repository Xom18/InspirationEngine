# InspirationEditor — 에디터 로드맵

별도 실행파일 프로젝트. `InspirationEngine.lib`를 링크하며 엔진 자산(IEWindow, IERenderer, IETextBox 등)을 재사용.  
게임 배포본에 툴 코드가 포함되지 않도록 엔진 lib와 분리.

```
InspirationEngine.sln
 ├── InspirationEngine  (lib)   — 엔진 런타임
 ├── InspirationEditor  (exe)   — 에디터 도구
 └── Example            (exe)   — 게임 샘플
```

---

## 현재 상태

| 단계 | 상태 | 내용 |
|------|------|------|
| E-0 프로젝트 설정 | ✅ | vcxproj + sln 등록 + main.cpp 뼈대 |
| E-1 Atlas Editor | 🔲 | PNG 로드 + 타일 분할 + JSON 저장 |
| E-2 파일 브라우저 | 🔲 | 경로 내 파일/폴더 트리 표시 |
| E-3 뷰포트 | 🔲 | IEScene 실시간 렌더링 창 |
| E-4 카메라 윈도우 | 🔲 | 카메라 타입 전환 + 파라미터 조정 |
| E-5 패널 시스템 | 🔲 | 도킹/분리 가능한 UI 패널 프레임워크 |

---

## E-0. 프로젝트 설정 ✅

**신규 파일**
- `InspirationEditor/InspirationEditor.vcxproj`
- `InspirationEditor/main.cpp`

**수정 파일**
- `InspirationEngine.sln` — InspirationEditor 프로젝트 추가

---

## E-1. Atlas Editor

**목표**: floors.json / ramps.json 와 동일한 포맷으로 타일 JSON 생성.  
PNG를 로드하고 그리드로 자동 분할 + 개별 타일 속성 편집 + JSON 저장.

### 출력 JSON 포맷 (IEAtlasManager 호환)
```json
{
  "image": "floors.png",
  "tileStep": { "x": 32, "y": 16 },
  "defaultPivotX": 16,
  "defaultPivotY": 8,
  "tiles": {
    "tile_r0_c0": { "x": 0,  "y": 0, "w": 32, "h": 16 },
    "tile_r0_c1": { "x": 32, "y": 0, "w": 32, "h": 16 }
  }
}
```

### UI 레이아웃 (별도 SDL 창 또는 메인 창 내 패널)
```
┌────────────────────────────────────────────┐
│ PNG: [경로__________] W:[32] H:[16] [로드]  │
├─────────────────────────┬──────────────────┤
│   PNG 캔버스             │ name: [________] │
│   그리드(회색)           │ x: y: w: h:      │
│   선택 타일(빨간 테두리)  │ pivotX: pivotY:  │
│                         │ [적용]           │
├─────────────────────────┴──────────────────┤
│ [JSON 저장]   tile 수: 0                    │
└────────────────────────────────────────────┘
```

### 기능 목록
- PNG 로드 (`IMG_Load` → SDL_Texture)
- W/H 입력 → 자동 분할 (`tile_r{r}_c{c}` 이름 부여)
- 타일 클릭 → 선택 강조
- 속성 패널에서 이름/좌표 편집 + [적용]
- [JSON 저장] → `nlohmann::json` 직렬화

---

## E-2. 파일 브라우저

**목표**: 지정 루트 경로의 파일/폴더를 트리로 표시. 파일 선택 시 해당 리소스 에디터 열기.

### 기능 목록
- 경로 입력 + 하위 디렉토리/파일 열거 (`std::filesystem::directory_iterator`)
- 파일 타입별 아이콘(텍스트로 대체 가능): `.png`, `.json`, `.wav` 등
- 더블클릭 → `.json` 이면 Atlas Editor 열기, `.png` 이면 미리보기

---

## E-3. 뷰포트

**목표**: IEScene 인스턴스를 에디터 창 내 별도 영역에 실시간 렌더링.

### 기능 목록
- 에디터에서 씬 파일(world_test.json) 로드
- 씬 오브젝트 렌더링 (기존 IEScene::Draw 재사용)
- 오브젝트 클릭 → 선택 + 속성 패널 연동

---

## E-4. 카메라 윈도우

**목표**: 현재 씬에 사용 중인 카메라 파라미터를 실시간 조정.

### 기능 목록
- 카메라 타입 선택 (TopView / Isometric / SideView / OverheadOblique / DepthSide)
- zoom, x, y 오프셋, tileWidth, tileHeight 등 파라미터 조정
- 변경 즉시 뷰포트 반영

---

## E-5. 패널 시스템

**목표**: UI 패널을 독립 SDL 창으로 분리하거나 메인 창에 도킹하는 프레임워크.

### 기능 목록
- `IEEditorPanel` 기반 클래스 — Draw / Update / SetRect
- 패널별 독립 창 생성 (IEWindow 상속)
- 드래그로 위치 이동
- 탭으로 패널 묶기 (선택)

---

## 추천 구현 순서

```
E-0 ✅ 프로젝트 설정
  └─► E-1 Atlas Editor          ← 즉시 사용 가능한 독립 도구
        └─► E-2 파일 브라우저   ← Atlas Editor와 연동
              └─► E-3 뷰포트    ← 씬 미리보기
                    └─► E-4 카메라 윈도우
                          └─► E-5 패널 시스템 (리팩터)
```
