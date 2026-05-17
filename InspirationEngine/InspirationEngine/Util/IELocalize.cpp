#include "../InspirationEngine.h"
#include <fstream>
#include <nlohmann/json.hpp>

std::unordered_map<std::string, std::string> IELocalize::s_table;

bool IELocalize::Load(const char* filePath)
{
    std::ifstream f(filePath);
    if (!f.is_open())
        return false;

    nlohmann::json j;
    try
    {
        f >> j;
    }
    catch (...)
    {
        return false;
    }

    s_table.clear();
    for (auto& [key, val] : j.items())
    {
        if (val.is_string())
            s_table[key] = val.get<std::string>();
    }
    return true;
}

const char* IELocalize::Get(const char* key)
{
    auto it = s_table.find(key);
    if (it != s_table.end())
        return it->second.c_str();
    return key;
}
