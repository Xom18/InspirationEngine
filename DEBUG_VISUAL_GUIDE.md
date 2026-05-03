# InspirationEngine — 게임 창 시각 디버깅 가이드

## 개요

카메라 시스템이 추가된 이후, 텍스트 콘솔(DebugWindow)만으로 월드 좌표·카메라 시야·오브젝트 위치를 파악하기 어려워짐.
게임 창을 직접 보면서 디버깅할 수 있도록 세 가지 기능을 제공.

---

## 기능 A — 인게임 오버레이 (F1 토글)

게임 렌더러 위에 직접 시각 정보를 그림. `MainWindow`에서 **F1** 을 누르면 토글.

표시 내용:
- **그리드**: 카메라가 있으면 월드 격자(64 유닛 간격)를 카메라 투영으로 그림. 없으면 픽셀 64 간격 그리드.
- **오브젝트 마커**: 활성 오브젝트의 스크린 좌표에 주황색 십자선 + 월드 좌표 텍스트.
- **카메라 HUD**: 좌상단에 카메라 위치, 줌, 뷰포트 크기 표시.

관련 파일:
- `InspirationEngine/Core/DebugInfo.h` — `DebugInfo::drawOverlay(renderer, scene, font)`
- `InspirationEngine/Core/DebugInfo.cpp` — 구현
- `Example/MainWindow.h` — `DebugInfo m_debug` 멤버
- `Example/MainWindow.cpp` — F1 토글, `m_debug.drawOverlay()` 호출

---

## 기능 B — 스크린샷 저장 (F2)

**F2** 를 누르면 게임 렌더러(renderer 1) 화면을 `debug_shot.png`로 저장.

```
저장 경로: InspirationEngine/Example/실행파일 위치/debug_shot.png
```

Claude가 파일 Read 툴로 이미지 직접 확인 가능.

관련 함수:
- `IERenderer::saveScreenshot(const char* path)` — SDL_RenderReadPixels + IMG_SavePNG

---

## 기능 C — MCP 스크린샷 서버 (C++ Win32, 선택)

외부 exe가 Win32 `PrintWindow` API로 게임 창을 캡처해 Claude에게 전달.
다른 창이 게임 창을 가려도 정확한 캡처 가능 (PIP 창 대응).

### 구현 계획

**구조**:
```
Claude Code ←→ (stdio JSON-RPC) ←→ ScreenshotMCP.exe ←→ Win32 PrintWindow
```

**VS 프로젝트**: 솔루션에 `ScreenshotMCP` 콘솔 앱 추가.

**의존성**: 순수 Win32 API + [nlohmann/json](https://github.com/nlohmann/json) (헤더 단일 파일).

**핵심 흐름**:
```
stdin → JSON 파싱 → "capture_window" 수신
→ FindWindowA(nullptr, title) → HWND
→ GetClientRect → CreateCompatibleDC + CreateDIBSection
→ PrintWindow(hwnd, hdc, PW_CLIENTONLY)
→ 픽셀 → BMP 바이트 → base64 인코딩
→ JSON-RPC response 출력
```

**MCP 도구**:
- `capture_window(title)` — 창 이름으로 캡처 → base64 BMP 반환
- `list_windows()` — 현재 창 목록 반환

**빌드**: `ScreenshotMCP/ScreenshotMCP.vcxproj` (솔루션에 포함됨)

**등록**: `.claude/settings.json` 에 이미 추가됨. Claude Code 재시작 시 자동 활성.

**on/off**: `.claude/settings.json` 항목 제거 또는 `"disabled": true`.

---

## 단축키 요약

| 키 | 기능 |
|----|------|
| F1 | 인게임 오버레이 토글 |
| F2 | `debug_shot.png` 저장 |
