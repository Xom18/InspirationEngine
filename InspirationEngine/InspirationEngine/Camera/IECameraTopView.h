#pragma once

#include "IECamera.h"

/// <summary>
/// 순수 탑뷰 카메라 (GTA2 스타일).
/// x,y → 스크린 x,y 직접 대응. z 완전 무시.
///
/// 투영 공식:
///   screenX = (wx - cx) * zoom + vw/2
///   screenY = (wy - cy) * zoom + vh/2
///
/// 정렬 키: y  (높은 y = 화면 아래 = viewer 에 가까움 = 나중에 그려짐)
/// </summary>
class IECameraTopView : public IECamera
{
public:
	IECameraTopView() = default;

	IEVector2 WorldToScreen(float wx, float wy, float wz = 0.0f) const override
	{
		IEVector2 result;
		result.m_x = std::lround((wx - m_x) * m_zoom + m_viewportWidth  * 0.5f);
		result.m_y = std::lround((wy - m_y) * m_zoom + m_viewportHeight * 0.5f);
		return result;
	}

	IEVector2 ScreenToWorld(int32_t sx, int32_t sy) const override
	{
		IEVector2 result;
		result.m_x = std::lround((sx - m_viewportWidth  * 0.5f) / m_zoom + m_x);
		result.m_y = std::lround((sy - m_viewportHeight * 0.5f) / m_zoom + m_y);
		return result;
	}

	// GetSortKey: 기본 y 그대로 사용 (IECamera 기본값)
};
