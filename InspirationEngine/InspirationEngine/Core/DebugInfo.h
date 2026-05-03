#pragma once

class IERenderer;
class IEScene;
class IECamera;
class Font;

/// <summary>
/// 게임 창 시각 디버깅 오버레이.
/// draw() 마지막에 drawOverlay() 호출, F1로 m_visible 토글.
/// </summary>
class DebugInfo
{
public:
	std::string m_log;
	bool        m_visible = false;

	/// <summary>
	/// 렌더러 위에 그리드·오브젝트 마커·카메라 HUD 오버레이
	/// </summary>
	void drawOverlay(IERenderer* renderer, IEScene* scene, Font* font);

private:
	void drawGrid(IERenderer* renderer, IECamera* camera);
	void drawObjectMarkers(IERenderer* renderer, IEScene* scene, IECamera* camera, Font* font);
	void drawCameraInfo(IERenderer* renderer, IECamera* camera, Font* font);
};
