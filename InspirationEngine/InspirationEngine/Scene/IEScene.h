#pragma once

class IEGameObject;
class IERenderer;

/// <summary>
/// 게임 오브젝트를 묶는 씬 기반 클래스.
/// update/draw 기본 구현은 m_objects 순회 + Y-sort.
/// </summary>
class IEScene
{
protected:
	std::vector<std::unique_ptr<IEGameObject>> m_objects;

public:
	virtual ~IEScene();	// .cpp에서 정의 — IEGameObject 완전 타입 보장

	virtual void onEnter() {}
	virtual void onExit() {}

	/// <summary>
	/// 활성 오브젝트 update 순회
	/// </summary>
	virtual void update(float deltaTime);

	/// <summary>
	/// Y-sort 후 활성 오브젝트 draw 순회
	/// </summary>
	virtual void draw(IERenderer* renderer);

	/// <summary>
	/// T 타입 오브젝트 생성 후 씬에 추가. 추가된 포인터 반환.
	/// </summary>
	template<typename T, typename... Args>
	T* addObject(Args&&... args)
	{
		static_assert(std::is_base_of_v<IEGameObject, T>, "T must derive from IEGameObject");
		std::unique_ptr<IEGameObject> obj(new T(std::forward<Args>(args)...));
		T* ptr = static_cast<T*>(obj.get());
		m_objects.push_back(std::move(obj));
		return ptr;
	}

	/// <summary>
	/// 포인터로 오브젝트 제거
	/// </summary>
	void removeObject(IEGameObject* obj);
};
