#include "InspirationEngine.h"

IESpriteManager::IESpriteManager()
{
}

IESpriteManager::~IESpriteManager()
{
	UnloadAll();
}

bool IESpriteManager::AddNewSprite(int32_t id, const char* path, IERenderer* renderer)
{
	SDL_Surface* pSurface = IMG_Load(path);
	if (pSurface == nullptr)
		return false;

	SDL_Texture* pTexture = SDL_CreateTextureFromSurface(renderer->GetSDLRenderer(), pSurface);
	SDL_DestroySurface(pSurface);

	if (pTexture == nullptr)
		return false;

	SDL_SetTextureScaleMode(pTexture, SDL_SCALEMODE_PIXELART);

	// 기존 ID 덮어쓰기
	auto iteT = m_textures.find(id);
	if (iteT != m_textures.end())
	{
		SDL_DestroyTexture(iteT->second);
		iteT->second = pTexture;
	}
	else
	{
		m_textures[id] = pTexture;
	}

	auto iteS = m_sprites.find(id);
	if (iteS != m_sprites.end())
	{
		iteS->second->SetTexture(pTexture);
	}
	else
	{
		auto pSprite = std::make_unique<IESprite>();
		pSprite->SetTexture(pTexture);
		m_sprites.try_emplace(id, std::move(pSprite));
	}

	return true;
}

void IESpriteManager::Unload(int32_t id)
{
	auto iteT = m_textures.find(id);
	if (iteT != m_textures.end())
	{
		SDL_DestroyTexture(iteT->second);
		m_textures.erase(iteT);
	}
	m_sprites.erase(id);
}

void IESpriteManager::UnloadAll()
{
	for (auto& [id, texture] : m_textures)
		SDL_DestroyTexture(texture);
	m_textures.clear();
	m_sprites.clear();
}

// ──────────────────────────────────────────────
//  IESprite
// ──────────────────────────────────────────────

void IESprite::SetTexture(SDL_Texture* texture)
{
	m_texture = texture;
	m_hasClip = false;
	if (texture != nullptr)
	{
		float fw = 0.0f, fh = 0.0f;
		SDL_GetTextureSize(texture, &fw, &fh);
		m_srcRect = { 0.0f, 0.0f, fw, fh };
	}
}

void IESprite::Draw(IERenderer* renderer, int32_t x, int32_t y,
	double scaleX, double scaleY, double angle, SDL_FPoint* pivot, SDL_FlipMode flip)
{
	if (m_texture == nullptr || renderer == nullptr)
		return;
	renderer->DrawTexture(m_texture, x, y, scaleX * 100.0, scaleY * 100.0, angle, pivot, flip, GetSrcRect());
}
