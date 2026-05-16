#include <fstream>
#include <nlohmann/json.hpp>
#include "IESceneSerializer.h"
#include "../InspirationEngine.h"

bool IESceneSerializer::Save(const IEScene& scene, const char* path)
{
    nlohmann::json j;
    j["version"] = "1.1";

    nlohmann::json objs = nlohmann::json::array();
    for (const auto& obj : scene.GetObjects())
    {
        nlohmann::json o;
        o["type"]   = obj->GetTypeName();
        o["active"] = obj->IsActive();
        if (!obj->GetName().empty())
            o["name"] = obj->GetName();

        auto* t = obj->GetComponent<IETransformComponent>();
        if (t != nullptr)
        {
            o["transform"] = {
                {"x",        t->GetX()},
                {"y",        t->GetY()},
                {"z",        t->GetZ()},
                {"rotation", t->GetRotation()},
                {"scaleX",   t->GetScaleX()},
                {"scaleY",   t->GetScaleY()}
            };
        }

        auto* camComp = obj->GetComponent<IECameraComponent>();
        if (camComp != nullptr)
        {
            o["camera"] = {
                {"cameraType",   IECameraComponent::TypeName(camComp->GetType())},
                {"zoom",         camComp->GetZoom()},
                {"tileW",        camComp->GetTileW()},
                {"tileH",        camComp->GetTileH()},
                {"heightFactor", camComp->GetHeightFactor()}
            };
        }

        auto* v = obj->GetComponent<IEVelocityComponent>();
        if (v != nullptr)
        {
            o["velocity"] = {
                {"vx", v->GetVx()},
                {"vy", v->GetVy()},
                {"vz", v->GetVz()}
            };
        }

        auto* tile = obj->GetComponent<IETileComponent>();
        if (tile != nullptr)
        {
            o["tile"] = {
                {"atlas", tile->GetAtlas()},
                {"tile",  tile->GetTile()}
            };
        }

        objs.push_back(std::move(o));
    }
    j["objects"] = std::move(objs);

    std::ofstream f(path);
    if (!f.is_open())
        return false;
    f << j.dump(2);
    return true;
}

bool IESceneSerializer::Load(IEScene& scene, const char* path)
{
    std::ifstream f(path);
    if (!f.is_open())
        return false;

    nlohmann::json j = nlohmann::json::parse(f, nullptr, false);
    if (j.is_discarded())
        return false;

    scene.Clear();

    if (!j.contains("objects") || !j["objects"].is_array())
        return true;

    for (auto& o : j["objects"])
    {
        std::string type = o.value("type", "StaticObject");
        IEGameObject* obj = nullptr;

        if (type == "Camera")
        {
            auto* camObj = scene.AddObject<IECameraObject>();
            scene.SetCameraObject(camObj);
            obj = camObj;

            if (o.contains("camera"))
            {
                auto& cd      = o["camera"];
                auto* camComp = obj->GetComponent<IECameraComponent>();
                if (camComp != nullptr)
                {
                    camComp->SetType(IECameraComponent::TypeFromName(cd.value("cameraType", "TopView")));
                    camComp->SetZoom(cd.value("zoom", 1.0f));
                    camComp->SetTileW(cd.value("tileW", 64));
                    camComp->SetTileH(cd.value("tileH", 32));
                    camComp->SetHeightFactor(cd.value("heightFactor", 0.5f));
                }
            }
        }
        else if (type == "Entity")
        {
            obj = scene.AddObject<IEEntity>();
        }
        else
        {
            obj = scene.AddObject<IEStaticObject>();
        }

        obj->SetActive(o.value("active", true));
        obj->SetName(o.value("name", std::string{}));

        if (o.contains("transform"))
        {
            auto& td = o["transform"];
            auto* t  = obj->GetComponent<IETransformComponent>();
            if (t != nullptr)
            {
                t->SetX(td.value("x", 0.0f));
                t->SetY(td.value("y", 0.0f));
                t->SetZ(td.value("z", 0.0f));
                t->SetRotation(td.value("rotation", 0.0f));
                t->SetScaleX(td.value("scaleX", 1.0f));
                t->SetScaleY(td.value("scaleY", 1.0f));
            }
        }

        if (o.contains("velocity"))
        {
            auto& vd = o["velocity"];
            auto* v  = obj->GetComponent<IEVelocityComponent>();
            if (v != nullptr)
            {
                v->SetVx(vd.value("vx", 0.0f));
                v->SetVy(vd.value("vy", 0.0f));
                v->SetVz(vd.value("vz", 0.0f));
            }
        }

        if (o.contains("tile"))
        {
            auto& ti = o["tile"];
            auto* tc = obj->GetComponent<IETileComponent>();
            if (tc == nullptr)
                tc = obj->AddComponent<IETileComponent>();
            tc->SetAtlas(ti.value("atlas", ""));
            tc->SetTile(ti.value("tile", ""));
        }
    }
    return true;
}
