#pragma once

#include "../Camera/IECamera.h"

class IEGameObject;
class IERenderer;

/// <summary>
/// 게임 오브젝트를 묶는 씬 기반 클래스.
/// SetCameraObject() 로 카메라 오브젝트 지정 시 매 Update에 자동 동기화.
/// SetCamera() (레거시) 또는 SetCameraObject() 중 하나를 사용.
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
	/// SyncCamera 후 활성 오브젝트 Update 순회
	/// </summary>
	virtual void Update(float deltaTime);

	/// <summary>
	/// 카메라 기반 정렬 후 활성 오브젝트 Draw 순회.
	/// overrideCam != nullptr 이면 씬 카메라 대신 해당 카메라로 렌더링.
	/// </summary>
	/// <param name="renderer">사용할 렌더러</param>
	/// <param name="overrideCam">뷰포트 오버라이드 카메라 (nullptr = 씬 카메라 사용)</param>
	virtual void Draw(IERenderer* renderer, IECamera* overrideCam = nullptr);

	/// <summary>
	/// 레거시 카메라 설정 — 소유권 이전. IECameraObject 미사용 시.
	/// </summary>
	void SetCamera(IECamera* camera) { m_camera.reset(camera); }

	/// <summary>
	/// 활성 카메라 반환 — 카메라 오브젝트 캐시 우선, 없으면 레거시 카메라
	/// </summary>
	IECamera* GetCamera() const
	{
		return (m_cameraCache != nullptr) ? m_cameraCache : m_camera.get();
	}

	/// <summary>
	/// 씬의 활성 카메라 오브젝트 지정 — IECameraComponent 필수
	/// </summary>
	void SetCameraObject(IEGameObject* obj);

	/// <summary>
	/// 활성 카메라 오브젝트 반환 (없으면 nullptr)
	/// </summary>
	IEGameObject* GetCameraObject() const { return m_cameraObject; }

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
	/// 씬의 모든 오브젝트 및 카메라 참조 제거
	/// </summary>
	void Clear()
	{
		m_objects.clear();
		m_cameraObject = nullptr;
		m_cameraCache  = nullptr;
	}

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
	void ClearViewportOverride() { m_vpOverrideW = 0; m_vpOverrideH = 0; }

private:
	/// <summary>카메라 오브젝트에서 위치·파라미터를 m_cameraCache 로 동기화</summary>
	void SyncCamera();

	std::vector<std::unique_ptr<IEGameObject>> m_objects;
	std::unique_ptr<IECamera>                  m_camera;        // 레거시
	IEGameObject*                              m_cameraObject = nullptr;  // 비소유
	IECamera*                                  m_cameraCache  = nullptr;  // 비소유
	int32_t                                    m_vpOverrideW  = 0;
	int32_t                                    m_vpOverrideH  = 0;
};
