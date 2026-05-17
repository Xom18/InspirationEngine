#include "InspirationEngine.h"

IEScene::~IEScene() = default;

void IEScene::SetCameraObject(IEGameObject* obj)
{
    m_cameraObject = obj;
    m_cameraCache  = nullptr;
}

void IEScene::SyncCamera()
{
    if (m_cameraObject == nullptr)
        return;
    auto* camComp = m_cameraObject->GetComponent<IECameraComponent>();
    auto* xfrm    = m_cameraObject->GetComponent<IETransformComponent>();
    if (camComp == nullptr)
        return;
    m_cameraCache = camComp->GetCamera();
    if (m_cameraCache != nullptr && xfrm != nullptr)
        m_cameraCache->SetPosition(xfrm->GetX(), xfrm->GetY());
}

void IEScene::Update(float deltaTime)
{
    SyncCamera();
    for (auto& obj : m_objects)
    {
        if (obj->IsActive())
            obj->Update(deltaTime);
    }
}

void IEScene::Draw(IERenderer* renderer, IECamera* overrideCam)
{
    IECamera* cam = (overrideCam != nullptr) ? overrideCam
                  : (m_cameraCache != nullptr ? m_cameraCache : m_camera.get());

    if (cam != nullptr)
    {
        int32_t vw = (m_vpOverrideW > 0) ? m_vpOverrideW : renderer->GetW();
        int32_t vh = (m_vpOverrideH > 0) ? m_vpOverrideH : renderer->GetH();
        cam->SetViewport(vw, vh);
    }

    std::vector<IEGameObject*> sorted;
    sorted.reserve(m_objects.size());
    for (auto& obj : m_objects)
    {
        if (obj->IsActive())
            sorted.push_back(obj.get());
    }

    if (cam != nullptr)
    {
        std::sort(sorted.begin(), sorted.end(), [cam](const IEGameObject* a, const IEGameObject* b)
        {
            auto* ta = a->GetComponent<IETransformComponent>();
            auto* tb = b->GetComponent<IETransformComponent>();
            float ka = ta != nullptr ? cam->GetSortKey(ta->GetX(), ta->GetY(), ta->GetZ()) : 0.0f;
            float kb = tb != nullptr ? cam->GetSortKey(tb->GetX(), tb->GetY(), tb->GetZ()) : 0.0f;
            return ka < kb;
        });
    }
    else
    {
        std::sort(sorted.begin(), sorted.end(), [](const IEGameObject* a, const IEGameObject* b)
        {
            return a->GetSortKey() < b->GetSortKey();
        });
    }

    for (auto* obj : sorted)
    {
        auto* t = obj->GetComponent<IETransformComponent>();

        int32_t sx, sy;
        if (cam != nullptr && t != nullptr)
        {
            auto pos = cam->WorldToScreen(t->GetX(), t->GetY(), t->GetZ());
            sx = pos.GetX();
            sy = pos.GetY();
        }
        else
        {
            sx = t != nullptr ? static_cast<int32_t>(t->GetX()) : 0;
            sy = t != nullptr ? static_cast<int32_t>(t->GetY()) : 0;
        }

        obj->Draw(renderer, sx, sy, cam != nullptr ? cam->GetZoom() : 1.0f);
    }
}

void IEScene::RemoveObject(IEGameObject* target)
{
    if (target == m_cameraObject)
    {
        m_cameraObject = nullptr;
        m_cameraCache  = nullptr;
    }
    auto it = std::find_if(m_objects.begin(), m_objects.end(),
        [target](const std::unique_ptr<IEGameObject>& p) { return p.get() == target; });
    if (it != m_objects.end())
        m_objects.erase(it);
}
