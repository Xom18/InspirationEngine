#pragma once

#include "IECamera.h"

/// <summary>
/// 뎁스 사이드뷰 카메라 (Dungeon & Fighter 스타일).
/// 사이드뷰이지만 y축이 화면 안쪽(멀어지는 방향)으로 뎁스를 형성한다.
///
/// 좌표 규약:
///   x+ = 오른쪽 (수평 이동)
///   y+ = 화면 안쪽 (멀어짐 — viewer 반대 방향)
///   z+ = 위 (점프 / 높이)
///
/// 투영 공식:
///   screenX = (wx - cx) * zoom + vw/2
///   screenY = (-(wy - cy) * depthFactor - (wz - cz) * heightFactor) * zoom + vh/2
///   ─ y+ 가 멀어질수록 화면 위쪽으로 이동 (먼 오브젝트 = 화면 상단)
///   ─ z+ 가 높아질수록 화면 위쪽으로 이동 (점프)
///
/// 정렬 키: -y
///   ─ y 가 클수록(멀수록) 낮은 정렬 키 → 먼저 그려짐(뒤)
/// </summary>
class IECameraDepthSide : public IECamera
{
	float m_depthFactor  = 0.5f;	// y 1단위 → 화면 픽셀 수직 이동량
	float m_heightFactor = 1.0f;	// z 1단위 → 화면 픽셀 수직 이동량

public:
	IECameraDepthSide() = default;
	IECameraDepthSide(float depthFactor, float heightFactor = 1.0f)
		: m_depthFactor(depthFactor), m_heightFactor(heightFactor) {}

	IVector2 worldToScreen(float wx, float wy, float wz = 0.0f) const override
	{
		IVector2 result;
		result.m_x = static_cast<int32_t>((wx - m_x) * m_zoom + m_viewportWidth  * 0.5f);
		result.m_y = static_cast<int32_t>((-(wy - m_y) * m_depthFactor - (wz - m_z) * m_heightFactor) * m_zoom + m_viewportHeight * 0.5f);
		return result;
	}

	// screenToWorld: z = 0 가정, y 복원
	IVector2 screenToWorld(int32_t sx, int32_t sy) const override
	{
		IVector2 result;
		result.m_x = static_cast<int32_t>((sx - m_viewportWidth  * 0.5f) / m_zoom + m_x);
		// z = 0: screenY = -wy * depthFactor * zoom + vh/2  →  wy = -(sy - vh/2) / (depthFactor * zoom)
		if (m_depthFactor != 0.0f)
			result.m_y = static_cast<int32_t>(-(sy - m_viewportHeight * 0.5f) / (m_depthFactor * m_zoom) + m_y);
		else
			result.m_y = 0;
		return result;
	}

	float getSortKey(float x, float y, float z) const override
	{
		// y 가 클수록 멀리 있음 → 먼저 그려짐 → 낮은 정렬 키
		return -y;
	}

	void  setDepthFactor(float f)  { m_depthFactor  = f; }
	void  setHeightFactor(float f) { m_heightFactor = f; }
	float getDepthFactor()  const  { return m_depthFactor; }
	float getHeightFactor() const  { return m_heightFactor; }
};
