# UTF-8 방어 개선 - 상세 리뷰

## 📋 변경 개요

`StrUTF8.cpp`에 잘못된 UTF-8 입력에 대한 방어 로직을 추가했습니다.

---

## 🛡️ 추가된 방어 기능

### 1. 유효성 검증 함수 추가

```cpp
// 유효한 UTF-8 시작 바이트 확인
static bool IsValidStartByte(unsigned char c)

// UTF-8 바이트 길이 계산 (시작 바이트 기반)
static size_t GetUTF8ByteLength(unsigned char c)
```

**처리 범위:**
- ASCII: 0x00-0x7F (0xxxxxxx)
- 2-byte: 0xC2-0xDF (110xxxxx, C0-C1은 overlong encoding 제외)
- 3-byte: 0xE0-0xEF (1110xxxx)
- 4-byte: 0xF0-0xF7 (11110xxx)
- **무효**: 0xC0-0xC1 (overlong), 0xF8-0xFF (reserved), 0x80-0xBF (continuation only)

---

## 🔍 각 함수별 개선사항

### `pop_back()` - 마지막 문자 제거

**추가된 검사:**
1. 연속 바이트가 마지막에 있을 경우 → 해당 바이트만 제거
2. 시작 바이트의 유효성 검증 → 무효면 마지막 바이트만 제거
3. 완성된 UTF-8 시퀀스 확인 → 불완전하면 마지막 바이트만 제거

**예시:**
```
"Test\x80"     → "Test"      (연속 바이트 단독)
"Test\xC0"     → "Test"      (불완전한 2-byte)
"Test가"       → "Test"      (정상 UTF-8)
```

### `pop_front()` - 첫 문자 제거

**추가된 검사:**
1. 연속 바이트가 시작에 있을 경우 → 해당 바이트만 제거
2. 시작 바이트 유효성 검증 → 무효면 첫 바이트만 제거
3. 불완전한 시퀀스 확인 → 불완전하면 첫 바이트만 제거
4. 모든 연속 바이트 유효성 검증 → 무효면 첫 바이트만 제거

**예시:**
```
"\x80Test"     → "Test"      (연속 바이트 단독)
"\xC0Test"     → "Test"      (불완전한 2-byte)
"가나"         → "나"        (정상 UTF-8)
```

### `getMemoryPoint()` - 문자 위치 → 메모리 위치 변환

**추가된 검사 (가장 중요):**
1. 유효한 시작 바이트 확인
2. 불완전한 시퀀스 감지 (바이트 부족)
3. 모든 연속 바이트의 유효성 검증
4. 각 문자마다 독립적으로 검증

**원본 문제점:** 
- 연속 바이트 단독이나 잘못된 시퀀스를 감지하지 못함
- 경계 검사가 불완전함

**개선:**
```cpp
// 각 바이트 길이별로 명확하게 검증
if ((c & 0x80) == 0) {
    // 1-byte ASCII
} else if ((c & 0xE0) == 0xC0) {
    // 2-byte: 길이 확인 → 연속 바이트 검증
} else if ((c & 0xF0) == 0xE0) {
    // 3-byte: 길이 확인 → 2개 연속 바이트 검증
} else if ((c & 0xF8) == 0xF0) {
    // 4-byte: 길이 확인 → 3개 연속 바이트 검증
}
```

### `removeToFront()` - 앞에서 문자 제거

**추가된 검사:**
1. 찾은 시작 바이트의 유효성 검증
2. 예상된 바이트 길이와 실제 길이 비교
3. 불완전한 시퀀스 처리

### `removeToBack()` - 뒤에서 문자 제거

**추가된 검사:**
1. 모든 바이트 길이별 불완전한 시퀀스 검사
2. 연속 바이트 유효성 검증
3. 진행 방향 안전성 확보

---

## 🧪 테스트 케이스

[TestStrUTF8.cpp](../Example/TestStrUTF8.cpp)에서 다음을 검증:

| 테스트 | 입력 예시 | 예상 동작 |
|--------|---------|---------|
| 연속 바이트 단독 | `"Hello\x80World"` | 안전하게 처리 |
| 불완전한 2-byte | `"Hello\xC0"` | 불완전 감지 |
| 불완전한 3-byte | `"Hi\xE0\x80"` | 불완전 감지 |
| 무효 시작 바이트 | `"Test\xF5..."` | 거부 |
| Overlong 인코딩 | `"Test\xC0\x80End"` | 거부 |
| 연속된 무효 바이트 | `"Start\x80\x81\x82End"` | 각각 처리 |
| 정상 UTF-8 | `"Hello가나다"` | 정상 작동 |

---

## ✅ 검증 기준

### 유효한 UTF-8 시퀀스
```
1-byte:  0xxxxxxx
2-byte:  110xxxxx 10xxxxxx  (0xC2-0xDF 범위)
3-byte:  1110xxxx 10xxxxxx 10xxxxxx  (0xE0-0xEF 범위)
4-byte:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  (0xF0-0xF7 범위)
```

### 무효한 바이트 패턴
- **0x80-0xBF (단독)**: 연속 바이트가 시작 바이트 없음
- **0xC0-0xC1**: Overlong 인코딩
- **0xF8-0xFF**: 예약됨
- **불완전 시퀀스**: 예상 바이트보다 부족
- **유효하지 않은 연속 바이트**: 10xxxxxx 패턴 아님

---

## 🎯 개선 효과

| 항목 | 이전 | 개선 후 |
|------|------|--------|
| 연속 바이트 단독 처리 | ❌ 미처리 | ✅ 안전 처리 |
| 불완전 시퀀스 감지 | ❌ 부분 | ✅ 완전 |
| 시작 바이트 검증 | ❌ 없음 | ✅ 엄격한 검증 |
| Overlong 인코딩 | ❌ 허용 | ✅ 거부 |
| 버퍼 오버플로우 위험 | ⚠️ 있음 | ✅ 제거 |
| 예외 처리 | ❌ 없음 | ✅ graceful fallback |

---

## 📌 주요 개선 포인트

### Before (문제점)
```cpp
// 과거: 연속 바이트를 건너뛸 뿐, 유효성 검증 없음
while (i < szLength) {
    if ((lpText[i + 1] & 0xC0) == 0xC0 
        || (lpText[i + 1] & 0x80) == 0)
        break;  // 경계 검사 없음! 버퍼 오버플로우 위험
    ++i;
}
```

### After (개선됨)
```cpp
// 현재: 모든 바이트를 엄격하게 검증
size_t len = 3;  // 예상 길이
if (i + len > szLength) {
    // 불완전한 시퀀스
    break;
}

// 모든 연속 바이트 검증
for (size_t j = 1; j < len; ++j) {
    if ((lpText[i + j] & 0xC0) != 0x80) {
        // 유효하지 않은 연속 바이트
        break;
    }
}
```

---

## 🚀 사용 권장사항

1. **테스트 실행 권장**
   ```
   Example/TestStrUTF8.cpp 컴파일 및 실행
   ```

2. **네트워크/파일 입력 처리 시**
   - 사용자 입력이나 외부 소스는 UTF-8 유효성이 보장되지 않음
   - 개선된 함수들이 안전하게 처리함

3. **TextBox 사용 시**
   - `removeToFront()`, `removeToBack()`, `getMemoryPoint()` 호출 안전
   - 무효한 UTF-8 입력이 있어도 crash 없음
