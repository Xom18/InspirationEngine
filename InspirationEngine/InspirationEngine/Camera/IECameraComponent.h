#pragma once
#include "IECamera.h"
#include "IECameraTopView.h"
#include "IECameraIsometric.h"
#include "IECameraSideView.h"
#include "IECameraOverheadOblique.h"
#include "IECameraDepthSide.h"
#include "../GameObject/IEComponent.h"
#include <memory>
#include <string>

/// <summary>
/// 카메라 타입·파라미터 컴포넌트.
/// GetCamera() 로 IECamera 구현체를 캐시하여 반환.
/// 타입·파라미터 변경 시 다음 GetCamera() 호출에서 인스턴스 재생성.
/// </summary>
class IECameraComponent : public IEComponent
{
public:
    enum class CameraType { TopView, Isometric, SideView, OverheadOblique, DepthSide };

    CameraType GetType()         const { return m_type; }
    float      GetZoom()         const { return m_zoom; }
    int32_t    GetTileW()        const { return m_tileW; }
    int32_t    GetTileH()        const { return m_tileH; }
    float      GetHeightFactor() const { return m_heightFactor; }

    void SetType(CameraType t)     { m_type = t;          m_dirty = true; }
    void SetZoom(float z)          { m_zoom = z;           m_dirty = true; }
    void SetTileW(int32_t w)       { m_tileW = w;          m_dirty = true; }
    void SetTileH(int32_t h)       { m_tileH = h;          m_dirty = true; }
    void SetHeightFactor(float hf) { m_heightFactor = hf;  m_dirty = true; }

    /// <summary>IECamera* 반환. 타입·파라미터 변경 시 내부 인스턴스 재생성.</summary>
    IECamera* GetCamera();

    /// <summary>직렬화용 타입 이름 문자열</summary>
    static const char*  TypeName(CameraType t);
    static CameraType   TypeFromName(const std::string& name);

private:
    CameraType               m_type         = CameraType::TopView;
    float                    m_zoom         = 1.0f;
    int32_t                  m_tileW        = 64;
    int32_t                  m_tileH        = 32;
    float                    m_heightFactor = 0.5f;
    bool                     m_dirty        = true;
    std::unique_ptr<IECamera> m_camera;
};
