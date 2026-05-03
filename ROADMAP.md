# InspirationEngine — 2D 게임엔진 완성 로드맵

## 현재 상태

| 시스템 | 상태 | 비고 |
|--------|------|------|
| SDL2 멀티 윈도우 | ✅ | 창별 독립 드로우 스레드 |
| 기본 렌더러 | ✅ | drawRect / Texture(srcRect) / Buffer / Surface / Text |
| 키보드 / 마우스 입력 | ✅ | SDL_Scancode 기반 |
| 텍스트 렌더링 | ✅ | FreeType + HarfBuzz, 마크업, 멀티라인 |
| 한글 IME | ✅ | TextBox 커서 / 그래핌 클러스터 |
| 폰트 관리자 | ✅ | 스타일별 FontFace (Bold, Italic 합성) |
| 스프라이트 관리자 | ✅ | IESpriteManager / IESprite, 스프라이트 시트 클리핑 |
| 아틀라스 관리자 | ✅ | IEAtlasManager — JSON 기반 타일 정의, top/side/anchor 지원 |
| 타일 컴포넌트 | ✅ | IETileComponent, IEStaticObject 타일 드로우 (zoom, anchor) |
| 엔진 코어 루프 | ✅ | 위상 기반 (Event → Update → Draw) |
| UTF-8 유틸리티 | ✅ | utf8proc 기반 |
| 파일 읽기 | ✅ | 바이너리 파일 로드 |

---

## 우선순위 1 — 기반 시스템 (없으면 게임을 만들 수 없다)

### ~~1-1. 스프라이트 리소스 관리자~~ ✅ 완료

`Sprite/IESpriteManager.h/.cpp` 구현 완료.

```
IESpriteManager  (IECore::m_Sprite 로 전역 접근)
  ├── addNewSprite(id, path, renderer)
  ├── getSprite(id) → IESprite*
  └── unload(id) / unloadAll()

IESprite
  ├── m_texture (SDL_Texture*, non-owning)
  ├── m_srcRect / m_hasClip (스프라이트 시트 클리핑)
  ├── draw(renderer, x, y, scaleX, scaleY, angle, pivot, flip)
  ├── setClip(x, y, w, h) / clearClip()
  └── getSrcRect() → nullptr if no clip

IERenderer::drawTexture (srcRect 파라미터 추가)
  └── const SDL_Rect* srcRect = nullptr  ← 기존 호출 전부 호환
```

---

### ~~1-2. 게임 오브젝트 시스템~~ ✅ 완료

`GameObject/IEComponent.h`, `IEGameObject.h`, `IEStaticObject.h/.cpp`, `IEEntity.h/.cpp` 구현 완료.

#### 좌표계

모든 월드 좌표 `float` 통일. 타일 *데이터*만 `int` 인덱스. z축(높이) 포함 3D 좌표계.

```
월드 (x, y, z: float)  →  [카메라 투영]  →  스크린 (screenX, screenY: int)
타일 인덱스 (tileX, tileY: int)  →  world position 계산 후 동일 파이프라인
```

#### ECS 컴포넌트 구조

```
IEComponent  (추상 기반)
  ├── typeId<T>()  — 타입당 정적 ID, 전역 단조 증가 카운터
  ├── IETransformComponent  x, y, z, rotation, scaleX, scaleY
  └── IEVelocityComponent   vx, vy, vz

IEGameObject  (추상 기반)
  ├── m_components  vector<unique_ptr<IEComponent>>  (인덱스 = typeId)
  ├── addComponent<T>()  — O(1), 중복 시 교체
  ├── getComponent<T>()  — O(1) 배열 인덱싱
  ├── m_sprite    (IESprite*, non-owning)
  ├── m_active    (bool)
  ├── update(deltaTime) — 기본 stub
  ├── draw(renderer)    — 순수 가상
  └── getSortKey()      — 기본 transform.y, isometric에서 오버라이드

IEStaticObject : IEGameObject
  ├── 생성자 → addComponent<IETransformComponent>() 자동
  └── draw() — transform 위치에 스프라이트 렌더

IEEntity : IEGameObject
  ├── 생성자 → Transform + Velocity 자동 추가
  ├── update(dt) — vx/vy/vz × dt 를 transform 에 적용
  └── draw()
```

#### 드로우 정렬 (Y-sort)

씬에서 렌더 전 `getSortKey()` 기준 정렬. 카메라 모드별 기본값:

| 카메라 모드 | sortKey |
|------------|---------|
| TopView | `y` |
| **Isometric** | `x + y - z × heightFactor` |
| SideView | `x` |
| OverheadOblique | `y - z × factor` |

---

### ~~1-3. 씬 관리자~~ ✅ 완료

`Scene/IEScene.h/.cpp`, `Scene/IESceneManager.h` 구현 완료.

```
IEScene
  ├── update(deltaTime) — 활성 오브젝트 순회
  ├── draw(renderer)    — Y-sort 후 순회
  ├── onEnter() / onExit()
  ├── addObject<T>()    — T 생성 후 소유권 획득, 포인터 반환
  ├── removeObject(ptr)
  └── vector<unique_ptr<IEGameObject>> m_objects

IESceneManager (IECore::m_Scene 전역 접근)
  ├── push(scene*) / pop() / replace(scene*)
  ├── getCurrentScene() → IEScene*
  ├── update(deltaTime) / draw(renderer)
  └── 스택 기반 — push/pop 시 onExit/onEnter 자동 호출
```

---

### ~~1-4. 델타 타임~~ ✅ 완료

`IECore::mainThread()` 에 `steady_clock` 기반 deltaTime 계산 추가.
`IECore::update(float deltaTime)` → 각 Window의 `update(float deltaTime)` 전파.
`IECore::getDeltaTime()` 정적 접근자로 어디서든 조회 가능.

```cpp
IECore::getDeltaTime()  → float  // 초 단위, 매 프레임 갱신
IEWindow::update(float deltaTime)  // virtual, 씬 연동 오버라이드 포인트
```

---

## 우선순위 2 — 핵심 게임플레이 기능

### 2-1. 카메라
**이유**: 월드 좌표 없이 모든 오브젝트가 화면 좌표를 직접 계산한다.

#### 목표 카메라 모드 4종

| 모드 | 레퍼런스 | screenX | screenY | 정렬 키 |
|------|---------|---------|---------|---------|
| **✅ TopView** | GTA2 | `(wx-cx)×zoom` | `(wy-cy)×zoom` | `y` |
| **✅ Isometric** | FF Tactics | `(x-y)×tw/2` | `(x+y)×th/2 - z×hf` | `(x+y)×th/2 - z×hf` |
| **✅ SideView** | 벨트스크롤 | `(wx-cx)×zoom` | `(wy-cy-wz×hf)×zoom` | `y - z×hf` |
| **✅ OverheadOblique** | Stoneshard / 구 Fallout | `(wx-cx)×zoom` | `(wy-cy-wz×hf)×zoom` | `y - z×hf` |
| **✅ DepthSide** | Dungeon & Fighter | `(wx-cx)×zoom` | `(-wy×df - wz×hf)×zoom` | `-y` |

```
IECamera (추상)                          Camera/IECamera.h
  ├── m_x, m_y, m_z, m_zoom, m_viewport
  ├── worldToScreen(x, y, z) → IVector2  (순수 가상)
  ├── screenToWorld(sx, sy)  → IVector2  (순수 가상, z=0 가정)
  ├── getSortKey(x, y, z)    → float     (기본: y)
  ├── follow(tx, ty, tz, lerpFactor, dt) — 선형 보간 추적
  └── setCamera() 로 IEScene 에 주입, 교체만으로 모드 전환

IECameraIsometric : IECamera             Camera/IECameraIsometric.h  ✅
  ├── m_tileWidth (64), m_tileHeight (32), m_heightFactor (16.0)
  ├── worldToScreen — 아이소 투영 + 카메라 오프셋 + zoom
  ├── screenToWorld — 역변환 (z=0 가정)
  └── getSortKey   — (x+y)×th/2 - z×hf

IECameraTopView         : IECamera  ✅   Camera/IECameraTopView.h
IECameraSideView        : IECamera  ✅   Camera/IECameraSideView.h
IECameraOverheadOblique : IECamera  ✅   Camera/IECameraOverheadOblique.h
IECameraDepthSide       : IECamera  ✅   Camera/IECameraDepthSide.h
```

#### 씬 연동 방식
- `IEScene::setCamera(new IECameraIsometric(64, 32))` — 소유권 이전
- `IEScene::draw()` 에서 자동으로 viewport 갱신 → sort → worldToScreen → 오브젝트 draw
- 오브젝트는 카메라를 전혀 모름 — `draw(renderer, screenX, screenY)` 만 수신

---

### 2-2. 충돌 감지
**이유**: 2D 게임의 핵심 게임플레이 로직.

```
IECollider (추상)
  ├── IEBoxCollider   (AABB)
  ├── IECircleCollider
  └── intersects(other) → bool

IECollisionManager
  ├── add(collider) / remove(collider)
  ├── checkAll() → vector<CollisionPair>
  └── 레이어 마스크 (선택)
```

- 물리 엔진 연동은 이후 (Box2D 등); 우선 단순 감지만

---

### 2-3. 오디오
**이유**: SDL_mixer가 이미 번들됨. 구현 비용 대비 효과 큼.

```
IEAudio (IECore 스태틱 멤버)
  ├── init(frequency, channels, chunkSize)
  ├── loadMusic(id, path) / loadSFX(id, path)
  ├── playMusic(id, loops) / stopMusic() / pauseMusic()
  ├── playSFX(id, volume)
  └── unload(id) / unloadAll()
```

- `Mix_Music*` / `Mix_Chunk*` ID 맵으로 관리 (폰트 관리자 패턴 동일)

---

## 우선순위 3 — UI / 렌더링 보완

### 3-1. 버튼 위젯
**이유**: `Menu` 기본 클래스가 있지만 비어있다. 게임 UI의 최소 단위.

```
IEButton : Menu
  ├── setState(Normal / Hover / Pressed / Disabled)
  ├── setLabel(text, font)
  ├── setSprite(normal, hover, pressed)
  ├── setCallback(std::function<void()>)
  └── update(mouseX, mouseY, mouseDown)
```

- `IECore::operateEvent()`에서 마우스 클릭 이벤트를 등록된 버튼들에 전달

---

### 3-2. 기하 도형 렌더러 확장
**이유**: 사각형만 있는 상태는 게임 디버그조차 어렵다.

```cpp
// IERenderer 추가
drawLine(color, x1, y1, x2, y2)
drawCircle(color, cx, cy, radius, filled)
drawPolygon(color, points[], count, filled)
```

- `drawLine` → `SDL_RenderDrawLine`
- `drawCircle` → 브레즌햄 원 알고리즘 (SDL_RenderGeometry 없이도 가능)
- `drawPolygon` → `SDL_RenderGeometry`

---

### 3-3. 마우스 커서 커스터마이즈
**이유**: `MouseCusor.h`가 비어있음. 커스텀 커서는 게임 UX 기본 요소.

```
IEMouseCursor
  ├── setSystemCursor(SDL_SystemCursor)
  ├── setCustomCursor(surface, hotX, hotY)
  ├── show() / hide()
  └── IEInput에 통합
```

---

## 우선순위 4 — 게임 콘텐츠 지원

### 4-1. 스프라이트 애니메이션
**이유**: 2D 게임의 핵심 표현 수단. 스프라이트 관리자(1-1) 완성 후 진행.

```
IEAnimation
  ├── vector<SDL_Rect> m_frames   (스프라이트 시트 상 프레임들)
  ├── float m_fps
  ├── bool m_loop
  └── update(deltaTime) → currentFrame

IEAnimator (IEGameObject 컴포넌트)
  ├── addClip(name, animation)
  ├── play(name) / stop() / pause()
  └── getCurrentSprite() → IESprite*
```

---

### 4-2. 타일맵
**이유**: RPG / 플랫포머 등 대부분의 2D 장르에서 필요.

```
IETileset
  ├── m_texture (스프라이트 시트)
  ├── m_tileWidth, m_tileHeight
  └── getRect(tileID) → SDL_Rect

IETilemap
  ├── vector<vector<int32_t>> m_tiles
  ├── draw(renderer, camera)
  └── Tiled (.tmx) 파일 파싱 (선택)
```

---

### 4-3. 파티클 시스템
**이유**: 타격감, 마법 이펙트 등 게임 연출의 핵심.

```
IEParticle
  ├── position, velocity, acceleration
  ├── lifetime, color, scale
  └── update(deltaTime)

IEParticleEmitter
  ├── spawn rate, burst count
  ├── 범위 / 방향 설정
  └── draw(renderer)
```

---

## 우선순위 5 — 엔진 품질 개선

### 5-1. 디버그 도구 강화
**이유**: `DebugInfo`에 로그 문자열만 있고 렌더링 연결이 없다.

- `DebugInfo`에 화면 렌더링 연결 (특정 윈도우 / 렌더러 지정)
- FPS 표시
- 충돌 박스 시각화 토글 (`isDebugDraw`)
- 로그 레벨 (Info / Warning / Error)

---

### 5-2. 설정 / 저장 시스템
**이유**: 게임 설정(해상도, 볼륨), 세이브 데이터 없이는 완성된 게임을 만들 수 없다.

```
IEConfig
  ├── load(path) / save(path)  → JSON 또는 INI
  ├── get<T>(key) / set<T>(key, value)
  └── IECore 스태틱 멤버로 추가

IESaveData
  ├── load(slot) / save(slot)
  └── get<T> / set<T>
```

---

### 5-3. 리소스 핫리로딩 (선택)
**이유**: 개발 반복 속도 향상. 텍스처 / 오디오 파일 수정 시 재시작 없이 반영.

- `IESpriteManager` / `IEAudio`에 파일 변경 감지 후 재로드 훅 추가

---

---

## 에디터 도구 (Editor Tools)

> **분리 원칙**: 에디터 도구는 엔진 라이브러리(`InspirationEngine.lib`)에 포함하지 않는다.  
> 게임 배포본에 툴 코드가 들어가지 않도록, 독립 실행파일 또는 별도 프로젝트로 분리.

```
InspirationEngine.sln
 ├── InspirationEngine  (lib) — 엔진 런타임, 툴 코드 없음
 ├── InspirationEditor        (exe) — 에디터 도구, 엔진 lib 링크
 └── Example            (exe) — 게임 샘플, 엔진 lib 링크
```

---

### E-1. Atlas Editor

**이유**: 아틀라스 JSON을 텍스트로 직접 편집하면 픽셀 좌표 실수가 잦다.  
PNG를 로드하고 타일을 GUI로 정의하는 전용 도구가 필요하다.

**프로젝트**: `InspirationEditor` (VS 솔루션 내 별도 exe)  
`InspirationEngine.lib` + `SDL2` + `SDL2_image` 링크.

#### 핵심 기능 (Phase 1 MVP)

| 기능 | 설명 |
|------|------|
| PNG 로드 | 경로 입력 → SDL_Surface 로드 후 캔버스 표시 |
| 자동 분할 | tileW / tileH 입력 → 그리드로 슬라이스, `tile_r{row}_c{col}` 이름 자동 부여 |
| 타일 선택 | 캔버스 클릭 → 해당 타일 강조 (빨간 테두리) |
| 속성 편집 | 선택된 타일의 name / x / y / w / h / anchorX / anchorY 수정 |
| JSON 저장 | floors.json / ramps.json 동일 포맷으로 파일 저장 |

#### UI 레이아웃

```
┌─────────────────────────────────────────┐
│ [PNG 경로]  [W:32] [H:32]  [로드/분할]   │
├──────────────────────┬──────────────────┤
│  PNG 캔버스           │  name: [______]  │
│  (줌 / 스크롤 지원)   │  x:    [______]  │
│  선택 타일 = 빨간 rect │  y:    [______]  │
│                      │  w:    [______]  │
│                      │  h:    [______]  │
│                      │  anchorX: [___]  │
│                      │  anchorY: [___]  │
│                      │  [적용]          │
├──────────────────────┴──────────────────┤
│  [JSON 저장]   타일 수: 0                │
└─────────────────────────────────────────┘
```

#### 신규 파일

| 파일 | 역할 |
|------|------|
| `InspirationEditor/main.cpp` | SDL 초기화 + 이벤트 루프 |
| `InspirationEditor/InspirationEditorApp.h/.cpp` | 캔버스 렌더 + 마우스 이벤트 + JSON 저장 |
| `InspirationEditor/InspirationEditor.vcxproj` | VS 프로젝트 (InspirationEngine.lib 링크) |

---

## 추천 진행 순서 요약

```
✅ 1-1 스프라이트 관리자
  └─► ✅ 1-2 게임 오브젝트
        └─► ✅ 1-3 씬 관리자
              └─► ✅ 1-4 델타 타임
                    ├─► 2-1 카메라
                    ├─► 2-2 충돌 감지
                    ├─► 2-3 오디오
                    ├─► 3-1 버튼 위젯
                    ├─► 3-2 기하 도형
                    └─► 4-1 애니메이션
                              └─► 4-2 타일맵
                                    └─► 4-3 파티클
```

**에디터 도구** (게임 기능과 독립적으로 진행 가능):
```
E-1 Atlas Editor  ← 현재 계획 중
```

> **현재 상태**: 카메라 5종 / 아틀라스 / 타일 드로우 완료. 다음은 충돌 감지(2-2) 또는 Atlas Editor(E-1).
