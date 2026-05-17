#include <fstream>
#include <nlohmann/json.hpp>
#include "IEProjectConfig.h"

float IEProjectConfig::s_camX       = 0.0f;
float IEProjectConfig::s_camY       = 0.0f;
float IEProjectConfig::s_camZoom    = 1.0f;
bool  IEProjectConfig::s_gridVisible = true;

bool IEProjectConfig::Load(const char* path)
{
    std::ifstream f(path);
    if (!f.is_open())
        return false;

    nlohmann::json j = nlohmann::json::parse(f, nullptr, false);
    if (j.is_discarded())
        return false;

    if (j.contains("camera"))
    {
        auto& c   = j["camera"];
        s_camX    = c.value("defaultX",    0.0f);
        s_camY    = c.value("defaultY",    0.0f);
        s_camZoom = c.value("defaultZoom", 1.0f);
    }

    if (j.contains("grid"))
    {
        auto& g      = j["grid"];
        s_gridVisible = g.value("visible", true);
    }

    return true;
}
