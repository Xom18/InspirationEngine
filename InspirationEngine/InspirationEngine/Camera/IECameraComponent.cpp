#include "../InspirationEngine.h"
#include "IECameraComponent.h"

IECamera* IECameraComponent::GetCamera()
{
    if (!m_dirty)
        return m_camera.get();

    switch (m_type)
    {
    case CameraType::TopView:
        m_camera = std::make_unique<IECameraTopView>();
        break;
    case CameraType::Isometric:
        m_camera = std::make_unique<IECameraIsometric>(m_tileW, m_tileH);
        break;
    case CameraType::SideView:
        m_camera = std::make_unique<IECameraSideView>();
        break;
    case CameraType::OverheadOblique:
        m_camera = std::make_unique<IECameraOverheadOblique>();
        break;
    case CameraType::DepthSide:
        m_camera = std::make_unique<IECameraDepthSide>();
        break;
    }
    if (m_camera != nullptr)
        m_camera->SetZoom(m_zoom);
    m_dirty = false;
    return m_camera.get();
}

const char* IECameraComponent::TypeName(CameraType t)
{
    switch (t)
    {
    case CameraType::TopView:         return "TopView";
    case CameraType::Isometric:       return "Isometric";
    case CameraType::SideView:        return "SideView";
    case CameraType::OverheadOblique: return "OverheadOblique";
    case CameraType::DepthSide:       return "DepthSide";
    }
    return "TopView";
}

IECameraComponent::CameraType IECameraComponent::TypeFromName(const std::string& name)
{
    if (name == "Isometric")       return CameraType::Isometric;
    if (name == "SideView")        return CameraType::SideView;
    if (name == "OverheadOblique") return CameraType::OverheadOblique;
    if (name == "DepthSide")       return CameraType::DepthSide;
    return CameraType::TopView;
}
