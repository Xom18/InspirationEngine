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

	/// <summary>
	/// 씬이 활성화될 때 호출 (파생 클래스에서 오버라이드)
	/// </summary>
	virtual void OnEnter() {}

	/// <summary>
	/// 씬이 비활성화될 때 호출 (파생 클래스에서 오버라이드)
	/// </summary>
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

	/// <summary>
	/// 씬 내 모든 오브젝트 목록 반환 (읽기 전용)
	/// </summary>
	const std::vector<std::unique_ptr<IEGameObject>>& GetObjects() const { return m_objects; }

	/// <summary>
	/// 씬의 모든 오브젝트 제거
	/// </summary>
	void Clear() { m_objects.clear(); }

	/// <summary>
	/// Draw 시 renderer 크기 대신 지정 크기를 카메라 뷰포트로 사용.
	/// 에디터 서브뷰포트 렌더링용.
	/// </summary>
	/// <param name="w">뷰포트 너비 (픽셀)</param>
	/// <param name="h">뷰포트 높이 (픽셀)</param>
	void SetViewportOverride(int32_t w, int32_t h) { m_vpOverrideW = w; m_vpOverrideH = h; }

	/// <summary>
	/// 뷰포트 오버라이드 제거 — renderer 실제 크기 사용으로 복귀
	/// </summary>
	void ClearViewportOverride()                   { m_vpOverrideW = 0; m_vpOverrideH = 0; }

private:
	std::vector<std::unique_ptr<IEGameObject>> m_objects;
	std::unique_ptr<IECamera>                  m_camera;
	int32_t                                    m_vpOverrideW = 0;
	int32_t                                    m_vpOverrideH = 0;
};
