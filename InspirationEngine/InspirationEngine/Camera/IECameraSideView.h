#pragma once

#include "IECamera.h"

/// <summary>
/// 사이드뷰 카메라 (벨트스크롤 스타일 — Streets of Rage, Final Fight).
/// x = 수평, y = 화면 깊이(奥行き), z = 수직 높이.
/// y 는 카메라 깊이 축 — 높은 y = 화면 위 = 먼 오브젝트.
///
/// 투영 공식:
///   screenX = (wx - cx) * zoom + vw/2
///   screenY = ((wy - cy) - (wz - cz) * heightFactor) * zoom + vh/2
///
/// 정렬 키: y - z * heightFactor
///   (y 가 높을수록 화면 위 = 먼 오브젝트 = 먼저 그려짐)
/// </summary>
class IECameraSideView : public IECamera
{
	float m_heightFactor = 1.0f;  // z 1단위 → 화면 픽셀 높이

public:
	IECameraSideView() = default;
	explicit IECameraSideView(float heightFactor) : m_heightFactor(heightFactor) {}

	IVector2 worldToScreen(float wx, float wy, float wz = 0.0f) const override
	{
		IVector2 result;
		result.m_x = static_cast<int32_t>((wx - m_x) * m_zoom + m_viewportWidth  * 0.5f);
		result.m_y = static_cast<int32_t>(((wy - m_y) - (wz - m_z) * m_heightFactor) * m_zoom + m_viewportHeight * 0.5f);
		return result;
	}

	// screenToWorld: z = 0 (지면 기준), y 복원
	IVector2 screenToWorld(int32_t sx, int32_t sy) const override
	{
		IVector2 result;
		result.m_x = static_cast<int32_t>((sx - m_viewportWidth  * 0.5f) / m_zoom + m_x);
		// z = 0 가정: screenY = (wy - cy) * zoom + vh/2
		result.m_y = static_cast<int32_t>((sy - m_viewportHeight * 0.5f) / m_zoom + m_y);
		return result;
	}

	float getSortKey(float x, float y, float z) const override
	{
		return y - z * m_heightFactor;
	}

	void  setHeightFactor(float f) { m_heightFactor = f; }
	float getHeightFactor() const  { return m_heightFactor; }
};
