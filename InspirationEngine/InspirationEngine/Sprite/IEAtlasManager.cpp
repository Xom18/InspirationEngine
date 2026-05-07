#include "InspirationEngine.h"
#include "nlohmann/json.hpp"
#include <fstream>

bool IEAtlasManager::Load(const std::string& name, const std::string& jsonPath, SDL_Renderer* renderer)
{
	std::ifstream f(jsonPath);
	if (!f.is_open())
		return false;

	nlohmann::json j = nlohmann::json::parse(f, nullptr, false);
	if (j.is_discarded())
		return false;

	if (!j.contains("image"))
		return false;
	std::string imagePath = j["image"].get<std::string>();

	std::string dir;
	auto slash = jsonPath.find_last_of("/\\");
	if (slash != std::string::npos)
		dir = jsonPath.substr(0, slash + 1);
	std::string fullImagePath = dir + imagePath;

	SDL_Surface* surf = IMG_Load(fullImagePath.c_str());
	if (surf == nullptr)
		return false;

	auto atlas = std::make_unique<IEAtlas>();
	atlas->m_texture = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);
	if (atlas->m_texture == nullptr)
		return false;

	if (j.contains("tileStep") && j["tileStep"].is_object())
	{
		atlas->m_tileStepX = j["tileStep"].value("x", 0);
		atlas->m_tileStepY = j["tileStep"].value("y", 0);
	}

	float defaultPivotX = j.value("defaultPivotX", 0.0f);
	float defaultPivotY = j.value("defaultPivotY", 0.0f);

	auto parseTileRect = [](const nlohmann::json& o) -> IETileRect {
		IETileRect r;
		r.x = o.value("x", 0);
		r.y = o.value("y", 0);
		r.w = o.value("w", 0);
		r.h = o.value("h", 0);
		return r;
	};

	if (j.contains("tiles") && j["tiles"].is_object())
	{
		for (auto& [tileName, tileData] : j["tiles"].items())
		{
			IETileDef def;
			if (tileData.contains("top"))
			{
				def.top     = parseTileRect(tileData["top"]);
				def.hasSide = tileData.contains("side");
				if (def.hasSide)
					def.side = parseTileRect(tileData["side"]);
			}
			else
			{
				def.top     = parseTileRect(tileData);
				def.hasSide = false;
			}
			def.pivotX = std::lround(tileData.value("pivotX", defaultPivotX));
			def.pivotY = std::lround(tileData.value("pivotY", defaultPivotY));
			atlas->m_tiles[tileName] = def;
		}
	}

	m_atlases[name] = std::move(atlas);
	return true;
}

void IEAtlasManager::Unload(const std::string& name)
{
	m_atlases.erase(name);
}

const IETileDef* IEAtlasManager::GetTile(const std::string& atlas, const std::string& tile) const
{
	auto it = m_atlases.find(atlas);
	if (it == m_atlases.end())
		return nullptr;
	auto jt = it->second->m_tiles.find(tile);
	if (jt == it->second->m_tiles.end())
		return nullptr;
	return &jt->second;
}

SDL_Texture* IEAtlasManager::GetTexture(const std::string& atlas) const
{
	auto it = m_atlases.find(atlas);
	if (it == m_atlases.end())
		return nullptr;
	return it->second->m_texture;
}

std::pair<int32_t, int32_t> IEAtlasManager::GetTileStep(const std::string& atlas) const
{
	auto it = m_atlases.find(atlas);
	if (it == m_atlases.end())
		return { 0, 0 };
	return { it->second->m_tileStepX, it->second->m_tileStepY };
}
