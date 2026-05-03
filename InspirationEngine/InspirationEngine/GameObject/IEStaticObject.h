#pragma once

#include "IEGameObject.h"

/// <summary>
/// 위치가 고정된 정적 오브젝트 — 배경, 타일, 장식물
/// IETransformComponent 자동 추가. update 불필요.
/// </summary>
class IEStaticObject : public IEGameObject
{
public:
	IEStaticObject()
	{
		addComponent<IETransformComponent>();
	}

	/// <summary>
	/// 스프라이트를 IETransformComponent 위치에 렌더링
	/// </summary>
	/// <param name="renderer">사용할 렌더러</param>
	void draw(IERenderer* renderer, int32_t screenX, int32_t screenY, float zoom = 1.0f) override;
};
