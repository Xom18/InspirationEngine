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
public:
	IECameraSideView() = default;
	explicit IECameraSideView(float heightFactor) : m_heightFactor(heightFactor) {}

	IEVector2 WorldToScreen(float wx, float wy, float wz = 0.0f) const override
	{
		IEVector2 result;
		result.SetX(static_cast<int32_t>(std::lround((wx - GetX()) * GetZoom() + GetViewportWidth()  * 0.5f)));
		result.SetY(static_cast<int32_t>(std::lround(((wy - GetY()) - (wz - GetZ()) * m_heightFactor) * GetZoom() + GetViewportHeight() * 0.5f)));
		return result;
	}

	IEVector2 ScreenToWorld(int32_t sx, int32_t sy) const override
	{
		IEVector2 result;
		result.SetX(static_cast<int32_t>(std::lround((sx - GetViewportWidth()  * 0.5f) / GetZoom() + GetX())));
		result.SetY(static_cast<int32_t>(std::lround((sy - GetViewportHeight() * 0.5f) / GetZoom() + GetY())));
		return result;
	}

	float GetSortKey(float x, float y, float z) const override
	{
		return y - z * m_heightFactor;
	}

	void  SetHeightFactor(float f) { m_heightFactor = f; }
	float GetHeightFactor() const  { return m_heightFactor; }

private:
	float m_heightFactor = 1.0f;
};
