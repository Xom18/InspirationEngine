#pragma once

#include "IEGameObject.h"

/// <summary>
/// 속도를 가진 동적 오브젝트 — 캐릭터, 투사체, NPC 등.
/// IETransformComponent + IEVelocityComponent 자동 추가.
/// 속도 조작: GetComponent<IEVelocityComponent>()->SetVx(...)
/// </summary>
class IEEntity : public IEGameObject
{
public:
	/// <summary>
	/// IETransformComponent, IEVelocityComponent를 자동 추가
	/// </summary>
	IEEntity()
	{
		AddComponent<IETransformComponent>();
		AddComponent<IEVelocityComponent>();
	}

	/// <summary>
	/// IEVelocityComponent 속도 × deltaTime 으로 위치 갱신
	/// </summary>
	/// <param name="deltaTime">경과 시간 (초)</param>
	virtual void Update(float deltaTime) override;

	/// <summary>
	/// 스프라이트를 IETransformComponent 위치에 렌더링
	/// </summary>
	/// <param name="renderer">사용할 렌더러</param>
	/// <param name="screenX">스크린 X</param>
	/// <param name="screenY">스크린 Y</param>
	/// <param name="zoom">카메라 확대 배율</param>
	virtual void Draw(IERenderer* renderer, int32_t screenX, int32_t screenY, float zoom = 1.0f) override;
};
