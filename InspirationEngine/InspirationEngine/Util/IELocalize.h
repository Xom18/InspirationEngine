#pragma once
#include <string>
#include <unordered_map>

/// <summary>
/// 현지화 문자열 테이블. Load() 후 Get()으로 키→문자열 조회.
/// 키가 없으면 키 자체를 반환하므로 누락 시에도 크래시 없음.
/// </summary>
class IELocalize
{
public:
    /// <summary>
    /// JSON 언어 파일 로드 (key-value 쌍). 기존 테이블을 교체.
    /// </summary>
    static bool        Load(const char* filePath);

    /// <summary>
    /// key에 대응하는 현지화 문자열 반환. 없으면 key 자체 반환.
    /// </summary>
    static const char* Get(const char* key);

private:
    static std::unordered_map<std::string, std::string> s_table;
};
