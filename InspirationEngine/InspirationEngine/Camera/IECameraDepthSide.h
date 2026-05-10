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
///
/// 정렬 키: -y
///   ─ y 가 클수록(멀수록) 낮은 정렬 키 → 먼저 그려짐(뒤)
/// </summary>
class IECameraDepthSide : public IECamera
{
public:
	IECameraDepthSide() = default;
	IECameraDepthSide(float depthFactor, float heightFactor = 1.0f)
		: m_depthFactor(depthFactor), m_heightFactor(heightFactor) {}

	IEVector2 WorldToScreen(float wx, float wy, float wz = 0.0f) const override
	{
		IEVector2 result;
		result.SetX(static_cast<int32_t>(std::lround((wx - GetX()) * GetZoom() + GetViewportWidth()  * 0.5f)));
		result.SetY(static_cast<int32_t>(std::lround((-(wy - GetY()) * m_depthFactor - (wz - GetZ()) * m_heightFactor) * GetZoom() + GetViewportHeight() * 0.5f)));
		return result;
	}

	IEVector2 ScreenToWorld(int32_t sx, int32_t sy) const override
	{
		IEVector2 result;
		result.SetX(static_cast<int32_t>((sx - GetViewportWidth()  * 0.5f) / GetZoom() + GetX()));
		if (m_depthFactor != 0.0f)
			result.SetY(static_cast<int32_t>(-(sy - GetViewportHeight() * 0.5f) / (m_depthFactor * GetZoom()) + GetY()));
		else
			result.SetY(0);
		return result;
	}

	float GetSortKey(float x, float y, float z) const override
	{
		return -y;
	}

	void  SetDepthFactor(float f)  { m_depthFactor  = f; }
	void  SetHeightFactor(float f) { m_heightFactor = f; }
	float GetDepthFactor()  const  { return m_depthFactor; }
	float GetHeightFactor() const  { return m_heightFactor; }

private:
	float m_depthFactor  = 0.5f;
	float m_heightFactor = 1.0f;
};
