#pragma once

#include "../Core/MacroDefine.h"

/// <summary>
/// 카메라 추상 기반 클래스.
/// 월드↔스크린 변환과 드로우 정렬 키를 정의한다.
/// IEScene::SetCamera() 로 씬에 주입하면 Draw/Sort 에 자동 적용.
/// </summary>
class IECamera
{
public:
	virtual ~IECamera() = default;

	/// <summary>
	/// 월드 좌표 → 스크린 픽셀 좌표
	/// </summary>
	virtual IEVector2 WorldToScreen(float wx, float wy, float wz = 0.0f) const = 0;

	/// <summary>
	/// 스크린 픽셀 좌표 → 월드 좌표 (z = 0 가정)
	/// </summary>
	virtual IEVector2 ScreenToWorld(int32_t sx, int32_t sy) const = 0;

	/// <summary>
	/// 드로우 정렬 키 — 낮을수록 먼저(뒤에) 그려짐.
	/// 기본: y (TopView / SideView 기준)
	/// </summary>
	virtual float GetSortKey(float x, float y, float z) const
	{
		return y;
	}

	/// <summary>
	/// 목표 월드 좌표로 선형 보간 이동
	/// </summary>
	void Follow(float tx, float ty, float tz, float lerpFactor, float deltaTime)
	{
		m_x += (tx - m_x) * lerpFactor * deltaTime;
		m_y += (ty - m_y) * lerpFactor * deltaTime;
		m_z += (tz - m_z) * lerpFactor * deltaTime;
	}

	void  SetPosition(float x, float y, float z = 0.0f) { m_x = x; m_y = y; m_z = z; }
	void  SetZoom(float zoom)                            { m_zoom = zoom; }
	void  SetViewport(int32_t w, int32_t h)              { m_viewportWidth = w; m_viewportHeight = h; }

	float   GetX()              const { return m_x; }
	float   GetY()              const { return m_y; }
	float   GetZ()              const { return m_z; }
	float   GetZoom()           const { return m_zoom; }
	int32_t GetViewportWidth()  const { return m_viewportWidth; }
	int32_t GetViewportHeight() const { return m_viewportHeight; }

private:
	float   m_x              = 0.0f;
	float   m_y              = 0.0f;
	float   m_z              = 0.0f;
	float   m_zoom           = 1.0f;
	int32_t m_viewportWidth  = 0;
	int32_t m_viewportHeight = 0;
};
