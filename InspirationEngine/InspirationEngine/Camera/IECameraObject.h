#pragma once
#include "../GameObject/IEGameObject.h"

/// <summary>
/// Hierarchy 에 표시되는 카메라 오브젝트.
/// IETransformComponent + IECameraComponent 자동 추가.
/// Draw() 는 비어있음 — 카메라는 씬 렌더링에 직접 표시되지 않음.
/// </summary>
class IECameraObject : public IEGameObject
{
public:
    /// <summary>IETransformComponent + IECameraComponent 자동 추가</summary>
    IECameraObject();

    virtual const char* GetTypeName() const override { return "Camera"; }

    virtual void Draw(IERenderer* /*renderer*/, int32_t /*screenX*/, int32_t /*screenY*/,
                      float /*zoom*/ = 1.0f) override {}
};
