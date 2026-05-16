#pragma once

class IEScene;
class IECamera;

/// <summary>
/// 씬 상태를 JSON 파일로 저장/복원.
/// 카메라는 "type":"Camera" 오브젝트로 objects 배열에 포함.
/// </summary>
class IESceneSerializer
{
public:
    /// <summary>
    /// 씬을 JSON 파일로 저장 (카메라 오브젝트 포함)
    /// </summary>
    /// <param name="scene">저장할 씬</param>
    /// <param name="path">출력 파일 경로</param>
    static bool Save(const IEScene& scene, const char* path);

    /// <summary>
    /// JSON 파일에서 씬 복원 (카메라 오브젝트 포함)
    /// </summary>
    /// <param name="scene">복원 대상 씬</param>
    /// <param name="path">입력 파일 경로</param>
    static bool Load(IEScene& scene, const char* path);

    // ── 레거시 호환 오버로드 (cam 파라미터 무시) ──────────────────────
    static bool Save(const IEScene& scene, const IECamera* /*cam*/, const char* path) { return Save(scene, path); }
    static bool Load(IEScene& scene, IECamera* /*cam*/, const char* path)             { return Load(scene, path); }
};
