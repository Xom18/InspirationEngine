#pragma once

#include "IEComponent.h"

class IERenderer;
class IESprite;

/// <summary>
/// 씬에 배치되는 모든 오브젝트의 추상 기반 클래스.
/// 컴포넌트를 조합해 오브젝트의 성격을 정의한다.
/// </summary>
class IEGameObject
{
public:
	virtual ~IEGameObject() = default;

	/// <summary>
	/// 컴포넌트 추가 — 타입당 1개, 중복 시 교체. 추가된 컴포넌트 포인터 반환.
	/// </summary>
	template<typename T, typename... Args>
	T* AddComponent(Args&&... args)
	{
		size_t id = IEComponent::TypeId<T>();
		if (id >= m_components.size())
			m_components.resize(id + 1);
		m_components[id] = std::make_unique<T>(std::forward<Args>(args)...);
		return static_cast<T*>(m_components[id].get());
	}

	/// <summary>
	/// 타입으로 컴포넌트 조회 — O(1) 배열 인덱싱, 없으면 nullptr
	/// </summary>
	template<typename T>
	T* GetComponent() const
	{
		size_t id = IEComponent::TypeId<T>();
		if (id >= m_components.size())
			return nullptr;
		return static_cast<T*>(m_components[id].get());
	}

	/// <summary>
	/// 프레임 업데이트
	/// </summary>
	/// <param name="deltaTime">경과 시간 (초)</param>
	virtual void Update(float /*deltaTime*/) {}

	/// <summary>
	/// 렌더러에 오브젝트 그리기 — 씬이 카메라 투영 후 스크린 좌표 전달
	/// </summary>
	/// <param name="renderer">사용할 렌더러</param>
	/// <param name="screenX">스크린 X (씬이 카메라 투영 결과 전달)</param>
	/// <param name="screenY">스크린 Y</param>
	/// <param name="zoom">카메라 확대 배율</param>
	virtual void Draw(IERenderer* renderer, int32_t screenX, int32_t screenY, float zoom = 1.0f) = 0;

	/// <summary>
	/// 드로우 정렬 키 반환 — 씬의 Y-sort 기준.
	/// 카메라가 있으면 카메라 GetSortKey 로 대체됨.
	/// </summary>
	virtual float GetSortKey() const
	{
		auto* t = GetComponent<IETransformComponent>();
		return t != nullptr ? t->GetY() : 0.0f;
	}

	/// <summary>
	/// 스프라이트 설정 (non-owning)
	/// </summary>
	/// <param name="sprite">사용할 스프라이트</param>
	void SetSprite(IESprite* sprite) { m_sprite = sprite; }

	/// <summary>
	/// 스프라이트 반환
	/// </summary>
	IESprite* GetSprite() const { return m_sprite; }

	/// <summary>
	/// 활성화 여부 설정
	/// </summary>
	/// <param name="active">활성화 여부</param>
	void SetActive(bool active) { m_active = active; }

	/// <summary>
	/// 활성화 여부 반환
	/// </summary>
	bool IsActive() const { return m_active; }

	/// <summary>
	/// 직렬화용 타입 이름 반환
	/// </summary>
	virtual const char* GetTypeName() const { return "GameObject"; }

private:
	std::vector<std::unique_ptr<IEComponent>> m_components;
	IESprite* m_sprite = nullptr;
	bool      m_active = true;
};
