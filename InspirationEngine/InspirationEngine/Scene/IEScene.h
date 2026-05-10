#pragma once

#include "../Camera/IECamera.h"

class IEGameObject;
class IERenderer;

/// <summary>
/// 게임 오브젝트를 묶는 씬 기반 클래스.
/// SetCamera() 로 카메라 주입 시 Draw/Sort 에 자동 적용.
/// 카메라 없으면 월드 좌표 = 스크린 좌표 (no-projection 모드).
/// </summary>
class IEScene
{
public:
	virtual ~IEScene();

	virtual void OnEnter() {}
	virtual void OnExit() {}

	/// <summary>
	/// 활성 오브젝트 Update 순회
	/// </summary>
	virtual void Update(float deltaTime);

	/// <summary>
	/// 카메라 기반 정렬 후 활성 오브젝트 Draw 순회
	/// </summary>
	virtual void Draw(IERenderer* renderer);

	/// <summary>
	/// 카메라 설정 — 소유권 이전. nullptr 로 제거 가능.
	/// </summary>
	void SetCamera(IECamera* camera)  { m_camera.reset(camera); }

	/// <summary>
	/// 현재 카메라 반환 (없으면 nullptr)
	/// </summary>
	IECamera* GetCamera() const { return m_camera.get(); }

	/// <summary>
	/// T 타입 오브젝트 생성 후 씬에 추가. 추가된 포인터 반환.
	/// </summary>
	template<typename T, typename... Args>
	T* AddObject(Args&&... args)
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
	void RemoveObject(IEGameObject* obj);

	const std::vector<std::unique_ptr<IEGameObject>>& GetObjects() const { return m_objects; }

private:
	std::vector<std::unique_ptr<IEGameObject>> m_objects;
	std::unique_ptr<IECamera>                  m_camera;
};
