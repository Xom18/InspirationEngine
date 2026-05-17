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
public:
	/// <summary>
	/// 기본 아이소메트릭 카메라 초기화 (tileW=64, tileH=32, heightFactor=16)
	/// </summary>
	IECameraIsometric() = default;

	/// <summary>
	/// 타일 크기와 높이 계수를 지정해 아이소메트릭 카메라 초기화
	/// </summary>
	/// <param name="tileW">타일 너비 (픽셀)</param>
	/// <param name="tileH">타일 높이 (픽셀)</param>
	/// <param name="heightFactor">z축 → 화면 Y 높이 변환 계수</param>
	IECameraIsometric(int32_t tileW, int32_t tileH, float heightFactor = 16.0f)
		: m_tileWidth(tileW), m_tileHeight(tileH), m_heightFactor(heightFactor) {}

	/// <summary>
	/// 월드 좌표를 아이소메트릭 스크린 좌표로 변환
	/// </summary>
	/// <param name="wx">월드 X</param>
	/// <param name="wy">월드 Y</param>
	/// <param name="wz">월드 Z (높이)</param>
	virtual IEVector2 WorldToScreen(float wx, float wy, float wz = 0.0f) const override
	{
		float hw = m_tileWidth  * 0.5f;
		float hh = m_tileHeight * 0.5f;

		float isoX = (wx - wy) * hw;
		float isoY = (wx + wy) * hh - wz * m_heightFactor;

		float camIsoX = (GetX() - GetY()) * hw;
		float camIsoY = (GetX() + GetY()) * hh - GetZ() * m_heightFactor;

		IEVector2 result;
		result.SetX(static_cast<int32_t>(std::lround((isoX - camIsoX) * GetZoom() + GetViewportWidth()  * 0.5f)));
		result.SetY(static_cast<int32_t>(std::lround((isoY - camIsoY) * GetZoom() + GetViewportHeight() * 0.5f)));
		return result;
	}

	/// <summary>
	/// 스크린 좌표를 월드 좌표로 역변환 (z = 0 가정)
	/// </summary>
	/// <param name="sx">스크린 X</param>
	/// <param name="sy">스크린 Y</param>
	virtual IEVector2 ScreenToWorld(int32_t sx, int32_t sy) const override
	{
		float hw = m_tileWidth  * 0.5f;
		float hh = m_tileHeight * 0.5f;

		float camIsoX = (GetX() - GetY()) * hw;
		float camIsoY = (GetX() + GetY()) * hh;

		float localX = (sx - GetViewportWidth()  * 0.5f) / GetZoom() + camIsoX;
		float localY = (sy - GetViewportHeight() * 0.5f) / GetZoom() + camIsoY;

		float diff = localX / hw;
		float sum  = localY / hh;

		IEVector2 result;
		result.SetX(static_cast<int32_t>((sum + diff) * 0.5f));
		result.SetY(static_cast<int32_t>((sum - diff) * 0.5f));
		return result;
	}

	/// <summary>
	/// 드로우 정렬 키 반환 — (x+y)*hh - z*heightFactor
	/// </summary>
	/// <param name="x">월드 X</param>
	/// <param name="y">월드 Y</param>
	/// <param name="z">월드 Z (높이)</param>
	virtual float GetSortKey(float x, float y, float z) const override
	{
		return (x + y) * (m_tileHeight * 0.5f) - z * m_heightFactor;
	}

	/// <summary>
	/// 타일 크기 설정
	/// </summary>
	/// <param name="w">타일 너비 (픽셀)</param>
	/// <param name="h">타일 높이 (픽셀)</param>
	void    SetTileSize(int32_t w, int32_t h) { m_tileWidth = w; m_tileHeight = h; }

	/// <summary>
	/// z축 높이 변환 계수 설정
	/// </summary>
	/// <param name="f">변환 계수</param>
	void    SetHeightFactor(float f)          { m_heightFactor = f; }

	/// <summary>
	/// 타일 너비 반환
	/// </summary>
	int32_t GetTileWidth()    const           { return m_tileWidth; }

	/// <summary>
	/// 타일 높이 반환
	/// </summary>
	int32_t GetTileHeight()   const           { return m_tileHeight; }

	/// <summary>
	/// z축 높이 변환 계수 반환
	/// </summary>
	float   GetHeightFactor() const           { return m_heightFactor; }

private:
	int32_t m_tileWidth    = 64;
	int32_t m_tileHeight   = 32;
	float   m_heightFactor = 16.0f;
};
