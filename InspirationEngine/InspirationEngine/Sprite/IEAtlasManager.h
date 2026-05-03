#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <SDL/SDL.h>

struct IETileRect {
	int32_t x = 0, y = 0, w = 0, h = 0;
	bool isEmpty() const { return w == 0 && h == 0; }
};

struct IETileDef {
	IETileRect top;
	IETileRect side;
	bool       hasSide = false;
	float      anchorX = 0.5f;
	float      anchorY = 0.5f;
};

class IEAtlas {
public:
	SDL_Texture* texture = nullptr;
	std::unordered_map<std::string, IETileDef> tiles;
	~IEAtlas() { if (texture) SDL_DestroyTexture(texture); }
};

class IEAtlasManager {
	std::unordered_map<std::string, std::unique_ptr<IEAtlas>> m_atlases;
public:
	bool load(const std::string& name, const std::string& jsonPath, SDL_Renderer* renderer);
	void unload(const std::string& name);

	const IETileDef* getTile(const std::string& atlas, const std::string& tile) const;
	SDL_Texture*     getTexture(const std::string& atlas) const;
};
