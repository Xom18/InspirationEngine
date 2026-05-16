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
	/// <summary>
	/// 기본 사이드뷰 카메라 초기화 (heightFactor = 1.0)
	/// </summary>
	IECameraSideView() = default;

	/// <summary>
	/// 높이 인수를 지정해 사이드뷰 카메라 초기화
	/// </summary>
	/// <param name="heightFactor">z축 → 화면 Y 변환 계수</param>
	explicit IECameraSideView(float heightFactor) : m_heightFactor(heightFactor) {}

	/// <summary>
	/// 월드 좌표를 스크린 좌표로 변환
	/// </summary>
	/// <param name="wx">월드 X</param>
	/// <param name="wy">월드 Y (깊이)</param>
	/// <param name="wz">월드 Z (높이)</param>
	virtual IEVector2 WorldToScreen(float wx, float wy, float wz = 0.0f) const override
	{
		IEVector2 result;
		result.SetX(static_cast<int32_t>(std::lround((wx - GetX()) * GetZoom() + GetViewportWidth()  * 0.5f)));
		result.SetY(static_cast<int32_t>(std::lround(((wy - GetY()) - (wz - GetZ()) * m_heightFactor) * GetZoom() + GetViewportHeight() * 0.5f)));
		return result;
	}

	/// <summary>
	/// 스크린 좌표를 월드 좌표로 역변환 (z = 0 가정)
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

	/// <summary>
	/// 드로우 정렬 키 반환 — y 가 클수록 위(멀리) = 먼저 그림
	/// </summary>
	/// <param name="y">월드 Y (깊이)</param>
	/// <param name="z">월드 Z (높이)</param>
	virtual float GetSortKey(float /*x*/, float y, float z) const override
	{
		return y - z * m_heightFactor;
	}

	/// <summary>
	/// z축 높이 변환 계수 설정
	/// </summary>
	/// <param name="f">변환 계수</param>
	void  SetHeightFactor(float f) { m_heightFactor = f; }

	/// <summary>
	/// z축 높이 변환 계수 반환
	/// </summary>
	float GetHeightFactor() const  { return m_heightFactor; }

private:
	float m_heightFactor = 1.0f;
};
