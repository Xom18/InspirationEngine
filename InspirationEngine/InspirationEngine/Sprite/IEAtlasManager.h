#pragma once
#include <string>
#include <unordered_map>
#include <memory>

struct IETileRect {
	int32_t x = 0, y = 0, w = 0, h = 0;
	bool isEmpty() const { return w == 0 && h == 0; }
};

struct IETileDef {
	IETileRect top;
	IETileRect side;
	bool       hasSide = false;
	int32_t    pivotX  = 0;     // 스프라이트 내 x 픽셀 오프셋 (screenX 정렬점)
	int32_t    pivotY  = 0;     // 스프라이트 내 y 픽셀 오프셋 (screenY 정렬점)
};

class IEAtlas {
	friend class IEAtlasManager;

	SDL_Texture*                               m_texture   = nullptr;
	std::unordered_map<std::string, IETileDef> m_tiles;
	int32_t                                    m_tileStepX = 0;
	int32_t                                    m_tileStepY = 0;

public:
	~IEAtlas() { if (m_texture != nullptr) SDL_DestroyTexture(m_texture); }
};

class IEAtlasManager {
	std::unordered_map<std::string, std::unique_ptr<IEAtlas>> m_atlases;
public:
	bool Load(const std::string& name, const std::string& jsonPath, SDL_Renderer* renderer);
	void Unload(const std::string& name);

	const IETileDef*             GetTile(const std::string& atlas, const std::string& tile) const;
	SDL_Texture*                 GetTexture(const std::string& atlas) const;
	std::pair<int32_t, int32_t>  GetTileStep(const std::string& atlas) const;
};
