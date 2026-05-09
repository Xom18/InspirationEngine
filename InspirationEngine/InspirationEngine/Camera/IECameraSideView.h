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
	float m_heightFactor = 1.0f;

public:
	IECameraSideView() = default;
	explicit IECameraSideView(float heightFactor) : m_heightFactor(heightFactor) {}

	IEVector2 WorldToScreen(float wx, float wy, float wz = 0.0f) const override
	{
		IEVector2 result;
		result.m_x = std::lround((wx - m_x) * m_zoom + m_viewportWidth  * 0.5f);
		result.m_y = std::lround(((wy - m_y) - (wz - m_z) * m_heightFactor) * m_zoom + m_viewportHeight * 0.5f);
		return result;
	}

	IEVector2 ScreenToWorld(int32_t sx, int32_t sy) const override
	{
		IEVector2 result;
		result.m_x = std::lround((sx - m_viewportWidth  * 0.5f) / m_zoom + m_x);
		result.m_y = std::lround((sy - m_viewportHeight * 0.5f) / m_zoom + m_y);
		return result;
	}

	float GetSortKey(float x, float y, float z) const override
	{
		return y - z * m_heightFactor;
	}

	void  SetHeightFactor(float f) { m_heightFactor = f; }
	float GetHeightFactor() const  { return m_heightFactor; }
};
