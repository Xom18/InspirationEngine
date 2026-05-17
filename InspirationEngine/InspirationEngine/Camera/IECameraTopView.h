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
	/// <summary>
	/// 기본 탑뷰 카메라 초기화
	/// </summary>
	IECameraTopView() = default;

	/// <summary>
	/// 월드 좌표를 스크린 좌표로 변환 (z 무시)
	/// </summary>
	/// <param name="wx">월드 X</param>
	/// <param name="wy">월드 Y</param>
	virtual IEVector2 WorldToScreen(float wx, float wy, float /*wz*/ = 0.0f) const override
	{
		IEVector2 result;
		result.SetX(static_cast<int32_t>(std::lround((wx - GetX()) * GetZoom() + GetViewportWidth()  * 0.5f)));
		result.SetY(static_cast<int32_t>(std::lround((wy - GetY()) * GetZoom() + GetViewportHeight() * 0.5f)));
		return result;
	}

	/// <summary>
	/// 스크린 좌표를 월드 좌표로 역변환
	/// </summary>
	/// <param name="sx">스크린 X</param>
	/// <param name="sy">스크린 Y</param>
	virtual IEVector2 ScreenToWorld(int32_t sx, int32_t sy) const override
	{
		IEVector2 result;
		result.SetX(static_cast<int32_t>(std::lround((sx - GetViewportWidth()  * 0.5f) / GetZoom() + GetX())));
		result.SetY(static_cast<int32_t>(std::lround((sy - GetViewportHeight() * 0.5f) / GetZoom() + GetY())));
		return result;
	}

	// GetSortKey: 기본 y 그대로 사용 (IECamera 기본값)
};
