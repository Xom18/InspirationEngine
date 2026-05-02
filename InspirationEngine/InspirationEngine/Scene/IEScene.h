#pragma once

#include "../Camera/IECamera.h"

class IEGameObject;
class IERenderer;

/// <summary>
/// 게임 오브젝트를 묶는 씬 기반 클래스.
/// setCamera() 로 카메라 주입 시 draw/sort 에 자동 적용.
/// 카메라 없으면 월드 좌표 = 스크린 좌표 (no-projection 모드).
/// </summary>
class IEScene
{
protected:
	std::vector<std::unique_ptr<IEGameObject>> m_objects;
	std::unique_ptr<IECamera>                  m_camera;

public:
	virtual ~IEScene();	// .cpp 정의 — IEGameObject 완전 타입 보장

	virtual void onEnter() {}
	virtual void onExit() {}

	/// <summary>
	/// 활성 오브젝트 update 순회
	/// </summary>
	virtual void update(float deltaTime);

	/// <summary>
	/// 카메라 기반 정렬 후 활성 오브젝트 draw 순회
	/// </summary>
	virtual void draw(IERenderer* renderer);

	/// <summary>
	/// 카메라 설정 — 소유권 이전. nullptr 로 제거 가능.
	/// </summary>
	void setCamera(IECamera* camera)  { m_camera.reset(camera); }

	/// <summary>
	/// 현재 카메라 반환 (없으면 nullptr)
	/// </summary>
	IECamera* getCamera() const { return m_camera.get(); }

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
