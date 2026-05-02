#pragma once

#include "IECamera.h"

/// <summary>
/// 아이소메트릭 쿼터뷰 카메라.
///
/// 월드 좌표계: x = 동쪽, y = 남쪽, z = 위 (높이축)
///
/// 투영 공식:
///   isoX = (wx - wy) * (tileW / 2)
///   isoY = (wx + wy) * (tileH / 2) - wz * heightFactor
///
/// 정렬 키:
///   (x + y) * (tileH / 2) - z * heightFactor
///   ─ 화면 아래 오브젝트(viewer에 가까움)가 나중에 그려짐
/// </summary>
class IECameraIsometric : public IECamera
{
	int32_t m_tileWidth    = 64;    // 타일 화면 너비 (px)
	int32_t m_tileHeight   = 32;    // 타일 화면 높이 (px)
	float   m_heightFactor = 16.0f; // z 1단위 → 화면 픽셀 높이

public:
	IECameraIsometric() = default;
	IECameraIsometric(int32_t tileW, int32_t tileH, float heightFactor = 16.0f)
		: m_tileWidth(tileW), m_tileHeight(tileH), m_heightFactor(heightFactor) {}

	IVector2 worldToScreen(float wx, float wy, float wz = 0.0f) const override
	{
		float hw = m_tileWidth  * 0.5f;
		float hh = m_tileHeight * 0.5f;

		// 오브젝트의 아이소 화면 좌표
		float isoX = (wx - wy) * hw;
		float isoY = (wx + wy) * hh - wz * m_heightFactor;

		// 카메라의 아이소 화면 좌표
		float camIsoX = (m_x - m_y) * hw;
		float camIsoY = (m_x + m_y) * hh - m_z * m_heightFactor;

		IVector2 result;
		result.m_x = static_cast<int32_t>((isoX - camIsoX) * m_zoom + m_viewportWidth  * 0.5f);
		result.m_y = static_cast<int32_t>((isoY - camIsoY) * m_zoom + m_viewportHeight * 0.5f);
		return result;
	}

	IVector2 screenToWorld(int32_t sx, int32_t sy) const override
	{
		float hw = m_tileWidth  * 0.5f;
		float hh = m_tileHeight * 0.5f;

		// 카메라 기준 아이소 로컬 좌표 (z = 0 가정)
		float camIsoX = (m_x - m_y) * hw;
		float camIsoY = (m_x + m_y) * hh;

		float localX = (sx - m_viewportWidth  * 0.5f) / m_zoom + camIsoX;
		float localY = (sy - m_viewportHeight * 0.5f) / m_zoom + camIsoY;

		// 역변환: localX = (rx-ry)*hw, localY = (rx+ry)*hh (z=0)
		float diff = localX / hw;  // wx - wy
		float sum  = localY / hh;  // wx + wy

		IVector2 result;
		result.m_x = static_cast<int32_t>((sum + diff) * 0.5f);
		result.m_y = static_cast<int32_t>((sum - diff) * 0.5f);
		return result;
	}

	float getSortKey(float x, float y, float z) const override
	{
		// 투영된 screenY 기준 — 화면 위에 있는 오브젝트(먼 것)가 먼저 그려짐
		return (x + y) * (m_tileHeight * 0.5f) - z * m_heightFactor;
	}

	void    setTileSize(int32_t w, int32_t h) { m_tileWidth = w; m_tileHeight = h; }
	void    setHeightFactor(float f)          { m_heightFactor = f; }
	int32_t getTileWidth()     const          { return m_tileWidth; }
	int32_t getTileHeight()    const          { return m_tileHeight; }
	float   getHeightFactor()  const          { return m_heightFactor; }
};
