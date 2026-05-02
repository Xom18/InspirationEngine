#pragma once

#include "IEGameObject.h"

/// <summary>
/// 속도를 가진 동적 오브젝트 — 캐릭터, 투사체, NPC 등
/// IETransformComponent + IEVelocityComponent 자동 추가.
/// 속도 조작: getComponent<IEVelocityComponent>()->vx = ...
/// </summary>
class IEEntity : public IEGameObject
{
public:
	IEEntity()
	{
		addComponent<IETransformComponent>();
		addComponent<IEVelocityComponent>();
	}

	/// <summary>
	/// IEVelocityComponent 속도 × deltaTime 으로 위치 갱신
	/// </summary>
	/// <param name="deltaTime">경과 시간 (초)</param>
	void update(float deltaTime) override;

	/// <summary>
	/// 스프라이트를 IETransformComponent 위치에 렌더링
	/// </summary>
	/// <param name="renderer">사용할 렌더러</param>
	void draw(IERenderer* renderer) override;
};
