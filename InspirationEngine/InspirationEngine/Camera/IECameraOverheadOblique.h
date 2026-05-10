#pragma once

#include "IECamera.h"

/// <summary>
/// 오버헤드 오블리크 카메라 (Stoneshard / 구 Fallout 스타일).
/// 탑뷰와 유사하지만 z(높이)가 오브젝트를 화면 위쪽으로 밀어 올려
/// 입체감을 표현한다. 실제 3D 원근은 없음.
///
/// 투영 공식:
///   screenX = (wx - cx) * zoom + vw/2
///   screenY = ((wy - cy) - (wz - cz) * heightFactor) * zoom + vh/2
///
/// 정렬 키: y - z * heightFactor
///   (높은 z = 화면 위로 이동 = viewer 에 가까워 보임 = 나중에 그려짐)
/// </summary>
class IECameraOverheadOblique : public IECamera
{
public:
	IECameraOverheadOblique() = default;
	explicit IECameraOverheadOblique(float heightFactor) : m_heightFactor(heightFactor) {}

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
