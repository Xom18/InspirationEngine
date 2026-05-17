#pragma once

/// <summary>
/// Data/project.json 기반 프로젝트 설정 — 카메라 기본값·그리드 표시 여부
/// </summary>
class IEProjectConfig
{
public:
	/// <summary>
	/// JSON 파일에서 프로젝트 설정 로드. 파일이 없으면 기본값 유지.
	/// </summary>
	/// <param name="path">설정 파일 경로</param>
	static bool Load(const char* path);

	/// <summary>기본 카메라 X 좌표</summary>
	static float GetDefaultCamX()    { return s_camX; }

	/// <summary>기본 카메라 Y 좌표</summary>
	static float GetDefaultCamY()    { return s_camY; }

	/// <summary>기본 카메라 줌</summary>
	static float GetDefaultCamZoom() { return s_camZoom; }

	/// <summary>그리드 표시 여부 기본값</summary>
	static bool  IsGridVisible()     { return s_gridVisible; }

private:
	static float s_camX;
	static float s_camY;
	static float s_camZoom;
	static bool  s_gridVisible;
};
