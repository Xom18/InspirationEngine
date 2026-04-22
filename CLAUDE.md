# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 빌드

Visual Studio 2022에서 `InspirationEngine/InspirationEngine.sln` 열고 빌드.

- `InspirationEngine` 프로젝트 → 정적 라이브러리 (.lib)
- `Example` 프로젝트 → 실행파일 (엔진 링크)
- C++20, MSVC v143, x86/x64 모두 지원
- 외부 의존성(SDL2, SDL2_ttf, SDL2_image, SDL2_mixer)은 `include/`, `lib/`, `dll/`에 번들됨

테스트 실행: Example 프로젝트 빌드 후 실행 → `TestStrUTF8` 함수가 콘솔에 결과 출력.

## 아키텍처

### 전체 구조

`cIECore` (전역 스태틱 싱글톤) → `cWindow` (창마다 별도 드로우 스레드) → `cRenderer` (SDL 렌더러 래퍼)

### 메인 루프 흐름

```
메인 스레드 (SDL 이벤트 폴링)
  ↓ eventPushBack()
cIECore::operateEvent()   ← SDL_Event 큐 처리, 각 Window에 분배
  ↓
cIECore::update()         ← 모든 Window의 update() 호출
  ↓
cIECore::draw()           ← cvDrawThreadWaiter.notify_all() 로 드로우 스레드 깨움
  ↓ (condition_variable 동기화)
Window::drawThread()      ← 창별 병렬 렌더링
  ↓ increaseDrawCompleteCount()
cIECore::draw()           ← cvDrawCompleteWaiter 대기 후 SDL_RenderPresent (메인 스레드)
```

엔진 페이즈: `eENGINE_PAHSE_OPERATE_EVENT` → `UPDATE` → `DRAW` → `COMPLETE` 순환.

### 창(Window) 시스템

- 창은 `cIECore::addNewWindow(id, new cWindow(...))` 으로 등록, 첫 번째 창이 자동으로 메인 창
- 창마다 드로우 스레드 1개, `cRenderer` 여러 개 보유 가능 (논리 해상도 != 물리 해상도)
- `m_lpMainWindow` 닫히면 엔진 종료

### UTF-8 처리 (`cStrUTF8`)

모든 문자열 조작은 `std::string`(UTF-8 바이트 배열)을 직접 다룸. `cStrUTF8`은 스태틱 유틸 클래스.

- `getMemoryPoint(str, charIndex)` → 논리적 문자 N번째의 바이트 오프셋 반환
- `IsValidStartByte()` / `GetUTF8ByteLength()` → public static 헬퍼 (내부 검증용)
- 무효 UTF-8(overlong, 단독 continuation byte, 불완전 시퀀스) → 1바이트 취급, crash 없음
- 상세 검증 기준: `IMPROVEMENT_UTF8_DEFENSE.md` 참조

### IME 지원

한국어/중국어/일본어 입력(IME)은 `cIECore::m_rtTextEditPosition`으로 후보창 위치를 SDL에 전달. `cTextBox`에서 포커스 변경 시 `updateTextEditPosition()` 호출.

### 클래스 접두어 규칙

- `c` prefix → 클래스 (예: `cWindow`, `cRenderer`)
- `m_lp` prefix → 포인터 멤버
- `m_i` prefix → int 멤버
- `_lp` / `_sz` / `_cs` → 함수 파라미터 (포인터 / size_t / const char*)
