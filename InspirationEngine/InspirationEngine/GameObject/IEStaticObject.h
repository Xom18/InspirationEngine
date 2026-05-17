#pragma once

#include "IEGameObject.h"

/// <summary>
/// 위치가 고정된 정적 오브젝트 — 배경, 타일, 장식물.
/// IETransformComponent 자동 추가. Update 불필요.
/// </summary>
class IEStaticObject : public IEGameObject
{
public:
	/// <summary>
	/// IETransformComponent를 자동 추가
	/// </summary>
	IEStaticObject()
	{
		AddComponent<IETransformComponent>();
	}

	virtual const char* GetTypeName() const override { return "StaticObject"; }

	/// <summary>
	/// 스프라이트 또는 타일을 IETransformComponent 위치에 렌더링
	/// </summary>
	/// <param name="renderer">사용할 렌더러</param>
	/// <param name="screenX">스크린 X</param>
	/// <param name="screenY">스크린 Y</param>
	/// <param name="zoom">카메라 확대 배율</param>
	virtual void Draw(IERenderer* renderer, int32_t screenX, int32_t screenY, float zoom = 1.0f) override;
};
