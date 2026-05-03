# InspirationEngine — 램프 아틀라스 + 인엔진 아틀라스 에디터

## Context

GameScene / floors.json / world_test.json 구현 완료.
다음 단계:
1. `ramps.png` 에 대응하는 `ramps.json` 템플릿 (빈 tiles, 에디터로 채울 예정)
2. 엔진 내장 Atlas Editor 도구 — 별도 SDL 창, `DebugWindow`와 동일 패턴으로 통합

---

## Part 1 — ramps.json 템플릿 (즉시 구현)

### 파일 위치
`InspirationEngine/Data/ramps.json`

### 내용
```json
{
  "image": "ramps.png",
  "tileStep": { "x": 32, "y": 16 },
  "defaultAnchorX": 0.5,
  "defaultAnchorY": 0.0,
  "tiles": {}
}
```

> tiles는 빈 상태. Atlas Editor 완성 후 auto-split으로 채움.  
> 기존 파서는 `tiles`가 비어도 정상 처리 (`IEAtlasManager::load` line 49 조건문 참조).

---

## Part 2 — 인엔진 Atlas Editor (로드맵)

### 목표 기능 (Phase 1 MVP)

| 기능 | 설명 |
|------|------|
| PNG 로드 | 경로 입력 → SDL_Surface 로드 |
| 자동 분할 | tileW/tileH 입력 → 그리드로 자동 슬라이스, `tile_r{row}_c{col}` 이름 부여 |
| 타일 클릭 | 캔버스에서 타일 클릭 → 선택 강조 |
| 속성 편집 | 선택된 타일의 이름 / x / y / w / h 수정 |
| JSON 저장 | floors.json 동일 포맷으로 파일 저장 |

### 아키텍처 — DebugWindow 패턴 재사용

```
IECore
 ├── IEWindow (game window)
 ├── DebugWindow           ← 기존 별도 SDL 창
 └── IEInspirationEditorWindow   ← 신규, 동일 구조
       m_sdlWindow
       m_renderer
       drawLoop()  ← 캔버스 + UI 그리기
       handleEvent() ← 마우스 클릭/드래그
```

`IEInspirationEditorWindow`는 독립 SDL 창. 엔진 루프와 분리된 별도 스레드 또는 동일 프레임 루프에서 처리.

### 신규 파일 목록

| 파일 | 역할 |
|------|------|
| `InspirationEngine/Editor/IEInspirationEditorWindow.h` | 에디터 창 선언 |
| `InspirationEngine/Editor/IEInspirationEditorWindow.cpp` | 에디터 로직 구현 |
| `InspirationEngine/InspirationEngine.h` | `#include "Editor/IEInspirationEditorWindow.h"` 추가 |

### UI 레이아웃 (SDL 드로우)

```
┌─────────────────────────────────────┐
│ [PNG 경로 입력]  [W:32] [H:16] [로드] │  ← 헤더
├──────────────────────┬──────────────┤
│                      │ 선택된 타일   │
│   PNG 캔버스          │ name: [___]  │
│   (줌/스크롤 지원)    │ x: [___]     │
│   선택 타일 = 빨간 테두리│ y: [___]  │
│                      │ w: [___]     │
│                      │ h: [___]     │
│                      │ anchorX: [_] │
│                      │ anchorY: [_] │
│                      │ [적용]       │
├──────────────────────┴──────────────┤
│ [JSON 저장]   tile 수: 0             │  ← 푸터
└─────────────────────────────────────┘
```

### 엔진 통합 방식

```cpp
// main.cpp 또는 GameScene에서 활성화
IECore::openInspirationEditor();  // 에디터 창 열기 (선택적)
```

또는 단축키(예: F3)로 토글.

---

## 구현 순서

1. `ramps.json` 템플릿 생성 → 즉시 완료 가능
2. `IEInspirationEditorWindow` 기본 뼈대 (SDL 창 + 이벤트 루프)
3. PNG 로드 + 캔버스 표시 + 자동 분할
4. 타일 선택 + 속성 편집 + JSON 저장

---

## 검증

1. `ramps.json` → `IECore::m_Atlas.load("ramps", ...)` 오류 없음, tile 수 0 정상
2. Atlas Editor 창 열기 → ramps.png 로드 + W32/H32 입력 → 자동 분할 타일 표시
3. 타일 클릭 → 이름 변경 → JSON 저장 → floors.json 동일 포맷 확인
