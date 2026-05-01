# InspirationEngine — 코딩 컨벤션

## 이름 규칙

### 클래스 / 구조체
- **PascalCase** 사용
- 엔진 라이브러리 클래스는 **`IE` 접두어** 필수
- 사유: 변수·일반 클래스·엔진 클래스를 이름만으로 구분

```cpp
// Good
class IECore { };
class IEWindow { };
class TestRunner { };
struct ShapedGlyph { };

// Bad
class cIECore { };   // c 접두어 금지
class Core { };      // 엔진 클래스에 IE 없음
```

### 변수
- **camelCase** 사용
- 타입을 나타내는 접두어 금지 (`lp`, `sz`, `cs`, `i`, `b` 등)
- 사유: 에디터가 타입을 표시해주므로 접두어 불필요

```cpp
// Good
int32_t tickRate;
bool isRunning;
std::string text;

// Bad
int m_iTickRate;   // 멤버도 아닌데 m_, 타입 접두어
BOOL bIsRunning;   // 타입 접두어
```

### 멤버 변수
- **`m_` 접두어** + camelCase
- 사유: 지역변수와 멤버변수를 한눈에 구분

```cpp
// Good
int32_t  m_tickRate;
bool     m_isRunning;
IEWindow* m_mainWindow;

// Bad
int32_t tickRate;    // m_ 없음
int32_t m_iTickRate; // 타입 접두어 중복
```

### 함수 / 메서드
- **PascalCase** 사용

```cpp
// Good
void BeginEngine();
bool IsRunning();
IEWindow* GetWindow(const char* id);

// Bad
void beginEngine();  // camelCase
void begin_engine(); // snake_case
```

### 파라미터
- **camelCase** 사용
- 타입 접두어 금지

```cpp
// Good
void CreateWindow(const char* title, int32_t width, int32_t height);

// Bad
void createWindow(const char* _csTitle, int _Width, int _Height);
```

### 파일명
- **클래스명과 동일** (PascalCase)
- 엔진 클래스는 IE 접두어 포함

```
IECore.h / IECore.cpp
IEWindow.h / IEWindow.cpp
Font.h / Font.cpp
StrUTF8.h / StrUTF8.cpp
```

### Enum
- `enum class` 강제, 타입은 명시적으로 지정
- 열거값은 **PascalCase**
- 사유: 스코프 있는 열거형으로 이름 충돌 방지, 암묵적 int 변환 차단

```cpp
// Good
enum class EnginePhase : int32_t
{
    NaN = 0,
    OperateEvent,
    Update,
    Draw,
    Complete
};
// 사용: EnginePhase::Update

// Bad
enum { eENGINE_PAHSE_UPDATE };  // 전역 오염, ALL_CAPS
```

---

## 타입

### 정수형
- `int` → `int32_t`, `unsigned int` → `uint32_t` 등 **`_t` 붙은 고정 크기 타입** 사용
- `size_t` 는 예외 (플랫폼에 맞는 크기를 의도적으로 사용하는 경우)
- `float`, `double`, `char` 등 실수형·문자형은 예외
- 사유: OS·아키텍처별 기본 정수 크기 차이 방지

```cpp
// Good
int32_t  tickRate;
uint32_t flags;
int64_t  timestamp;
size_t   length;
float    scale;

// Bad
int      tickRate;
unsigned flags;
UINT32   windowId;  // SDL/Windows 타입 직접 사용 금지 (인터페이스 경계 제외)
```

### 포인터 / 스마트 포인터
- 가급적 **스마트 포인터** 사용
  - 단독 소유: `std::unique_ptr`
  - 공유 소유: `std::shared_ptr`
  - 소유 없는 참조: `std::weak_ptr`
- 날 포인터(`*`)는 소유권 없는 참조 또는 외부 라이브러리 인터페이스 경계에서만 허용
- 사유: 수동 메모리 관리 오류 방지

```cpp
// Good
std::unique_ptr<IEWindow> m_mainWindow;
std::shared_ptr<Font>     m_font;
IEWindow*                 m_focusedWindow;  // 소유 없는 참조는 raw 허용

// Bad
IEWindow* m_mainWindow = new IEWindow();   // 수동 관리
```

### 캐스팅
- C 스타일 캐스팅 금지, **C++ 형변환 연산자** 사용
- 사유: 캐스팅 위치를 검색하기 쉽고 의도를 명확히 함

```cpp
// Good
int32_t x = static_cast<int32_t>(floatValue);
IEWindow* w = dynamic_cast<IEWindow*>(basePtr);

// Bad
int32_t x = (int32_t)floatValue;
```

---

## 코드 스타일

### nullptr
- `NULL`, `0` 대신 **`nullptr`** 사용

```cpp
// Good
IEWindow* w = nullptr;
if (ptr == nullptr)

// Bad
IEWindow* w = NULL;
if (!ptr)
```

### override
- 가상 함수 재정의 시 **`override`** 키워드 필수

```cpp
// Good
void Draw() override;
void Update() override;

// Bad
void Draw();  // override 없음
```

### 헤더 가드
- `#pragma once` 사용

### include 순서
1. 표준 라이브러리 (`<iostream>`, `<string>`, ...)
2. 서드파티 라이브러리 (`<SDL/SDL.h>`, ...)
3. 엔진 헤더 (`InspirationEngine.h`, ...)
4. 로컬 헤더 (`"MainWindow.h"`, ...)

```cpp
#include <string>
#include <vector>

#include <SDL/SDL.h>

#include "../InspirationEngine/InspirationEngine.h"

#include "MainWindow.h"
```

### constexpr
- 컴파일 타임 상수는 `#define` 대신 **`constexpr`** 사용

```cpp
// Good
constexpr int32_t g_screenWidth  = 1280;
constexpr int32_t g_screenHeight = 720;

// Bad
#define SCREEN_WIDTH 1280
```
