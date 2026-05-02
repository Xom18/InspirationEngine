#pragma once

#include "IEComponent.h"

class IERenderer;
class IESprite;

/// <summary>
/// 씬에 배치되는 모든 오브젝트의 추상 기반 클래스
/// 컴포넌트를 조합해 오브젝트의 성격을 정의한다
/// </summary>
class IEGameObject
{
	std::vector<std::unique_ptr<IEComponent>> m_components;

protected:
	IESprite* m_sprite = nullptr;	// non-owning
	bool m_active = true;

public:
	virtual ~IEGameObject() = default;

	/// <summary>
	/// 컴포넌트 추가 — 소유권 이전, 추가된 컴포넌트 포인터 반환
	/// </summary>
	template<typename T, typename... Args>
	T* addComponent(Args&&... args)
	{
		auto c = std::make_unique<T>(std::forward<Args>(args)...);
		T* ptr = c.get();
		m_components.push_back(std::move(c));
		return ptr;
	}

	/// <summary>
	/// 타입으로 컴포넌트 조회 — 없으면 nullptr
	/// </summary>
	template<typename T>
	T* getComponent() const
	{
		for (const auto& c : m_components)
			if (auto* p = dynamic_cast<T*>(c.get()))
				return p;
		return nullptr;
	}

	/// <summary>
	/// 프레임 업데이트
	/// </summary>
	/// <param name="deltaTime">경과 시간 (초)</param>
	virtual void update(float deltaTime) {}

	/// <summary>
	/// 렌더러에 오브젝트 그리기
	/// </summary>
	/// <param name="renderer">사용할 렌더러</param>
	virtual void draw(IERenderer* renderer) = 0;

	/// <summary>
	/// 드로우 정렬 키 반환 — 씬의 Y-sort 기준
	/// isometric 씬에서는 x + y - z × factor 로 오버라이드
	/// </summary>
	virtual float getSortKey() const
	{
		auto* t = getComponent<IETransformComponent>();
		return t ? t->y : 0.0f;
	}

	/// <summary>
	/// 스프라이트 설정 (non-owning)
	/// </summary>
	/// <param name="sprite">사용할 스프라이트</param>
	void setSprite(IESprite* sprite)
	{
		m_sprite = sprite;
	}

	/// <summary>
	/// 스프라이트 반환
	/// </summary>
	IESprite* getSprite() const
	{
		return m_sprite;
	}

	/// <summary>
	/// 활성화 여부 설정
	/// </summary>
	/// <param name="active">활성화 여부</param>
	void setActive(bool active)
	{
		m_active = active;
	}

	/// <summary>
	/// 활성화 여부 반환
	/// </summary>
	bool isActive() const
	{
		return m_active;
	}
};
