#include "../InspirationEngine.h"
#include "IECameraObject.h"

IECameraObject::IECameraObject()
{
    AddComponent<IETransformComponent>();
    AddComponent<IECameraComponent>();
}
