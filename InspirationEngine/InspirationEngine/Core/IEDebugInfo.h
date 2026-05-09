#pragma once

class IERenderer;
class IEScene;
class IECamera;
class IEFont;

/// <summary>
/// 게임 창 시각 디버깅 오버레이.
/// Draw() 마지막에 DrawOverlay() 호출, F1로 visible 토글.
/// </summary>
class IEDebugInfo
{
	std::string m_log;
	bool        m_visible = false;

public:
	/// <summary>
	/// 렌더러 위에 그리드·오브젝트 마커·카메라 HUD 오버레이
	/// </summary>
	/// <param name="renderer">사용할 렌더러</param>
	/// <param name="scene">현재 씬</param>
	/// <param name="font">HUD 텍스트용 폰트</param>
	void DrawOverlay(IERenderer* renderer, IEScene* scene, IEFont* font);

	/// <summary>
	/// 로그 문자열 반환
	/// </summary>
	const std::string& GetLog() const { return m_log; }

	/// <summary>
	/// 로그 문자열 설정
	/// </summary>
	/// <param name="log">설정할 로그</param>
	void SetLog(const std::string& log) { m_log = log; }

	/// <summary>
	/// 오버레이 표시 여부 반환
	/// </summary>
	bool IsVisible() const { return m_visible; }

	/// <summary>
	/// 오버레이 표시 여부 설정
	/// </summary>
	/// <param name="visible">표시 여부</param>
	void SetVisible(bool visible) { m_visible = visible; }

private:
	void DrawGrid(IERenderer* renderer, IECamera* camera);
	void DrawObjectMarkers(IERenderer* renderer, IEScene* scene, IECamera* camera, IEFont* font);
	void DrawCameraInfo(IERenderer* renderer, IECamera* camera, IEFont* font);
};
