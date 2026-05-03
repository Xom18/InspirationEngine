#include "InspirationEngine.h"
#include "nlohmann/json.hpp"
#include <fstream>

bool IEAtlasManager::load(const std::string& name, const std::string& jsonPath, SDL_Renderer* renderer)
{
	std::ifstream f(jsonPath);
	if (!f.is_open()) return false;

	nlohmann::json j;
	try { f >> j; }
	catch (...) { return false; }

	if (!j.contains("image")) return false;
	std::string imagePath = j["image"].get<std::string>();

	std::string dir;
	auto slash = jsonPath.find_last_of("/\\");
	if (slash != std::string::npos)
		dir = jsonPath.substr(0, slash + 1);
	std::string fullImagePath = dir + imagePath;

	SDL_Surface* surf = IMG_Load(fullImagePath.c_str());
	if (!surf) return false;

	auto atlas = std::make_unique<IEAtlas>();
	atlas->texture = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);
	if (!atlas->texture) return false;

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
				def.anchorX = tileData.value("anchorX", 0.5f);
				def.anchorY = tileData.value("anchorY", 0.5f);
			}
			else
			{
				def.top     = parseTileRect(tileData);
				def.hasSide = false;
				def.anchorX = tileData.value("anchorX", 0.5f);
				def.anchorY = tileData.value("anchorY", 0.5f);
			}
			atlas->tiles[tileName] = def;
		}
	}

	m_atlases[name] = std::move(atlas);
	return true;
}

void IEAtlasManager::unload(const std::string& name)
{
	m_atlases.erase(name);
}

const IETileDef* IEAtlasManager::getTile(const std::string& atlas, const std::string& tile) const
{
	auto it = m_atlases.find(atlas);
	if (it == m_atlases.end()) return nullptr;
	auto jt = it->second->tiles.find(tile);
	if (jt == it->second->tiles.end()) return nullptr;
	return &jt->second;
}

SDL_Texture* IEAtlasManager::getTexture(const std::string& atlas) const
{
	auto it = m_atlases.find(atlas);
	if (it == m_atlases.end()) return nullptr;
	return it->second->texture;
}
