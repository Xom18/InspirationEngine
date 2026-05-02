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

### 1-2. 게임 오브젝트 시스템
**이유**: 씬에 배치할 단위가 없으면 게임 구조를 잡을 수 없다.

#### 좌표계 설계 결정사항

모든 월드 좌표는 `float` 통일. 타일 *데이터*만 `int` 인덱스. z축(높이) 포함 3D 좌표계.

```
월드 (x, y, z: float)  →  [카메라 투영]  →  스크린 (screenX, screenY: int)
타일 인덱스 (tileX, tileY: int)  →  world position 계산 후 동일 파이프라인
```

#### 오브젝트 분류

| 분류 | 클래스 | 특징 |
|------|--------|------|
| 정적 | `IEStaticObject` | 배경·타일. update 없음. 위치 고정. |
| 동적 | `IEEntity` | 캐릭터·투사체. 속도·가속도 보유. |

```
IETransform
  ├── x, y, z   (float — z = 높이축)
  ├── rotation  (float, 도)
  └── scaleX, scaleY (float)

IEGameObject  (추상 기반)
  ├── m_transform (IETransform)
  ├── m_sprite    (IESprite*, non-owning)
  ├── m_active    (bool)
  ├── update(deltaTime) — 기본 stub
  ├── draw(renderer)    — 순수 가상
  └── getSortKey()      — 드로우 정렬 키 (카메라 모드에 따라 다름)

IEStaticObject : IEGameObject
  └── draw() — 트랜스폼 위치에 스프라이트 렌더

IEEntity : IEGameObject
  ├── m_velX, m_velY, m_velZ (float)
  ├── update(dt) — 속도 × dt 적용
  └── draw()
```

#### 드로우 정렬 (Y-sort)

씬에서 렌더 전 `getSortKey()` 기준 정렬 필요. 카메라 모드별 기본값:

| 카메라 모드 | sortKey |
|------------|---------|
| TopView | `y` |
| **Isometric** | `x + y - z × heightFactor` |
| SideView | `x` (좌→우 순서) |
| OverheadOblique | `y - z × factor` |

- `IEWindow::draw()` 가상 함수를 통해 씬이 오브젝트들을 순회하는 구조
- 상속 기반 (컴포넌트 패턴은 이후 필요 시 도입)

---

### 1-3. 씬 관리자
**이유**: 오브젝트 묶음 단위 없이는 스테이지 전환, 초기화 흐름을 관리할 수 없다.

```
IEScene
  ├── update(deltaTime)
  ├── draw(renderer)
  ├── onEnter() / onExit()
  └── vector<unique_ptr<IEGameObject>> m_objects

IESceneManager (IECore 멤버)
  ├── push(scene) / pop() / replace(scene)
  └── getCurrentScene() → IEScene*
```

---

### 1-4. 델타 타임
**이유**: 현재 고정 tick(16ms)만 있어 물리·애니메이션이 프레임률에 묶인다.

```cpp
// IECore::mainThread() 에 추가
float deltaTime = elapsedMs / 1000.0f;  // 초 단위
// IECore::update(float deltaTime) 으로 시그니처 변경
// IEGameObject::update(float deltaTime) 전파
```

---

## 우선순위 2 — 핵심 게임플레이 기능

### 2-1. 카메라
**이유**: 월드 좌표 없이 모든 오브젝트가 화면 좌표를 직접 계산한다.

#### 목표 카메라 모드 4종

| 모드 | 레퍼런스 | screenX | screenY |
|------|---------|---------|---------|
| **TopView** | GTA2 | `worldX` | `worldY` |
| **Isometric** | FF Tactics | `(x - y) × tw/2` | `(x + y) × th/2 - z × hf` |
| **SideView** | 벨트스크롤 | `worldX` | `-worldZ` |
| **OverheadOblique** | Binding of Isaac | `worldX` | `worldY - z × factor` |

```
IECamera (추상)
  ├── m_x, m_y, m_z   (float — 월드 위치)
  ├── m_zoom           (float)
  ├── worldToScreen(x, y, z) → (screenX, screenY)
  ├── screenToWorld(sx, sy)  → (worldX, worldY)
  └── follow(target, lerpFactor)

IECameraTopView      : IECamera
IECameraIsometric    : IECamera  ← 현재 목표
IECameraSideView     : IECamera
IECameraOverheadOblique : IECamera
```

- `IEScene`이 카메라를 보유, 렌더 시 `worldToScreen` 호출 후 오브젝트에 전달
- `IEWindow::screenPosToRenderPos`와 연계
- 카메라 모드 변경 = `IEScene::setCamera()` 교체만으로 완료

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

## 추천 진행 순서 요약

```
✅ 1-1 스프라이트 관리자
  └─► 1-2 게임 오브젝트
        └─► 1-3 씬 관리자
              └─► 1-4 델타 타임
                    ├─► 2-1 카메라
                    ├─► 2-2 충돌 감지
                    ├─► 2-3 오디오
                    ├─► 3-1 버튼 위젯
                    ├─► 3-2 기하 도형
                    └─► 4-1 애니메이션
                              └─► 4-2 타일맵
                                    └─► 4-3 파티클
```

> **다음 목표**: 1-2 게임 오브젝트 시스템 → 씬까지 완성하면 Example 프로젝트에서 실제 게임 프로토타입 제작 가능.
