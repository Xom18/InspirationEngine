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

// Bad
IEWindow* w = NULL;
IEWindow* w = 0;
```

### 조건식 비교
- **bool**: 비교식 없이 직접 사용 허용
- **pointer**: `== nullptr` / `!= nullptr` 비교식 명시 필수
- 사유: pointer를 `if (ptr)` 로 쓰면 bool·null 체크가 구분되지 않아 의도가 불명확해짐

```cpp
// Good
if (m_isRunning)          // bool — 비교식 생략 허용
if (!m_isActive)          // bool — 부정도 허용
if (ptr == nullptr)       // pointer — 비교식 명시
if (ptr != nullptr)       // pointer — 비교식 명시

// Bad
if (ptr)                  // pointer를 bool처럼 사용 금지
if (!ptr)                 // pointer를 bool처럼 사용 금지
if (m_isRunning == true)  // bool에 == true 불필요
```

### 내용이 한줄인 if
- 한줄일 경우 괄호 생략가능, 단 if와 동일한 줄에 분기동작 금지

```cpp
// Good
if (m_isCheck) // return과 같이 if 다음에 한줄일 경우 줄바꿈 / 괄호 생략
  return;

// Bad
if (m_isCheck) return; // 줄바꿈까지 생략 금지
```

### virtual
- 가상 함수 선언 시 **`virtual`** 키워드 명시 필수 (기저 클래스와 파생 클래스 모두)
- 사유: `override` 만으로는 해당 함수가 가상임을 선언부에서 즉시 알 수 없음

```cpp
// Good
virtual void Draw() override;
virtual void Update() override;

// Bad
void Draw() override;   // virtual 생략 — 가상임이 불명확
```

### override
- 가상 함수 재정의 시 **`override`** 키워드 필수

```cpp
// Good
virtual void Draw() override;
virtual void Update() override;

// Bad
virtual void Draw();  // override 없음 — 기저 클래스 시그니처 변경 시 묵시적 숨김 발생
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

### 클래스 내부 함수에 대한 설명
class와 그 내부 함수에는 반드시 주석으로 <summary>와 <param>에 대한 코멘트를 남겨준다

### 클래스 멤버 변수
멤버 변수는 private을 원칙으로 한다. 파생 클래스에서 직접 접근이 필요한 경우에도 getter/setter를 사용한다. public·protected 멤버 변수 금지.

### 클래스 내부 선언 순서
클래스 내부의 선언 순서는 아래를 따른다. `private:` 섹션은 클래스 기본 접근이 private이더라도 반드시 명시한다.

```cpp
class ClassName
{
    friend class OtherClass;  // friend 관계 (가급적 사용 금지)

public:
    ClassName();              // 생성자 / 소멸자
    ~ClassName();

    void PublicFunction();    // public 함수

protected:
    void ProtectedFunction(); // protected 함수

private:
    void PrivateFunction();   // private 함수

private:
    int32_t m_member = 0;    // private 멤버 변수 (항상 맨 아래)
};
```

- `friend` 선언은 클래스 최상단에 위치시키되, 가급적 사용하지 않는다.
- 생성자·소멸자는 public 함수 블록 최상단에 배치한다.
- 멤버 변수는 항상 `private:` 아래 맨 마지막 섹션에 모아서 배치한다.
- 비어있는 접근 지정자 섹션은 작성하지 않는다.

---

### try-catch 사용 배제
네트워크, 외부 파일 파싱 등 **예외가 불가피한 경계**가 아니라면 try-catch 사용 금지.
엔진 내부 로직에서의 try-catch는 예외를 숨겨 디버깅을 어렵게 만든다.
대신 반환값(`bool`, 에러 코드) 또는 로그로 처리한다.

```cpp
// Good — 반환값으로 처리
bool Load(const std::string& path)
{
    std::ifstream f(path);
    if (!f.is_open())
    {
        IELog::Error("파일 열기 실패: " + path);
        return false;
    }
    // ...
    return true;
}

// Bad — 엔진 내부에서 예외 은폐
try { DoSomething(); } catch (...) { }
```

### assert 사용 배제
`assert`는 릴리즈 빌드에서 제거되므로 의존하지 않는다.
조건 검사가 필요하면 명시적 조건문 + 로그로 대체한다.

```cpp
// Good
if (ptr == nullptr)
{
    IELog::Error("ptr이 nullptr — 초기화 누락 가능성");
    return;
}

// Bad
assert(ptr != nullptr);
```