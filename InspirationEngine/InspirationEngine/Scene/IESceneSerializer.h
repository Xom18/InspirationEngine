#pragma once

class IEScene;
class IECamera;

/// <summary>
/// 씬과 카메라 상태를 JSON 파일로 저장/복원
/// </summary>
class IESceneSerializer
{
public:
	/// <summary>
	/// 씬과 카메라 상태를 JSON 파일로 저장
	/// </summary>
	/// <param name="scene">저장할 씬</param>
	/// <param name="cam">저장할 카메라 (nullptr 가능)</param>
	/// <param name="path">출력 파일 경로</param>
	static bool Save(const IEScene& scene, const IECamera* cam, const char* path);

	/// <summary>
	/// JSON 파일에서 씬과 카메라 상태를 복원
	/// </summary>
	/// <param name="scene">복원 대상 씬</param>
	/// <param name="cam">복원 대상 카메라 (nullptr 가능)</param>
	/// <param name="path">입력 파일 경로</param>
	static bool Load(IEScene& scene, IECamera* cam, const char* path);
};
