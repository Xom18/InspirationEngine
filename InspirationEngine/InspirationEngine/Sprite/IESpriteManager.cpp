#include "InspirationEngine.h"

IESpriteManager::IESpriteManager()
{
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
}

IESpriteManager::~IESpriteManager()
{
	unloadAll();
	IMG_Quit();
}

bool IESpriteManager::addNewSprite(int32_t id, const char* path, IERenderer* renderer)
{
	SDL_Surface* pSurface = IMG_Load(path);
	if (pSurface == nullptr)
		return false;

	SDL_Texture* pTexture = SDL_CreateTextureFromSurface(renderer->getSDLRenderer(), pSurface);
	SDL_FreeSurface(pSurface);

	if (pTexture == nullptr)
		return false;

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
		iteS->second->setTexture(pTexture);
	}
	else
	{
		auto pSprite = std::make_unique<IESprite>();
		pSprite->setTexture(pTexture);
		m_sprites.try_emplace(id, std::move(pSprite));
	}

	return true;
}

void IESpriteManager::unload(int32_t id)
{
	auto iteT = m_textures.find(id);
	if (iteT != m_textures.end())
	{
		SDL_DestroyTexture(iteT->second);
		m_textures.erase(iteT);
	}
	m_sprites.erase(id);
}

void IESpriteManager::unloadAll()
{
	for (auto& [id, texture] : m_textures)
		SDL_DestroyTexture(texture);
	m_textures.clear();
	m_sprites.clear();
}

// ──────────────────────────────────────────────
//  IESprite
// ──────────────────────────────────────────────

void IESprite::setTexture(SDL_Texture* texture)
{
	m_texture = texture;
	m_hasClip = false;
	if (texture != nullptr)
	{
		int32_t w = 0, h = 0;
		SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
		m_srcRect = { 0, 0, w, h };
	}
}

void IESprite::draw(IERenderer* renderer, int32_t x, int32_t y,
	double scaleX, double scaleY, double angle, SDL_Point* pivot, SDL_RendererFlip flip)
{
	if (m_texture == nullptr || renderer == nullptr)
		return;
	renderer->drawTexture(m_texture, x, y, scaleX * 100.0, scaleY * 100.0, angle, pivot, flip, getSrcRect());
}
