#include "InspirationEngine.h"

void IEStaticObject::Draw(IERenderer* renderer, int32_t screenX, int32_t screenY, float zoom)
{
	if (!m_active || renderer == nullptr)
		return;

	auto* tc = GetComponent<IETileComponent>();
	if (tc != nullptr)
	{
		const IETileDef* def = IECore::GetAtlas().GetTile(tc->GetAtlas(), tc->GetTile());
		SDL_Texture*     tex = IECore::GetAtlas().GetTexture(tc->GetAtlas());
		if (def == nullptr || tex == nullptr)
			return;

		int32_t dw = std::lround(def->top.w * zoom);
		int32_t dh = std::lround(def->top.h * zoom);
		int32_t dx = screenX - std::lround(def->pivotX * zoom);
		int32_t dy = screenY - std::lround(def->pivotY * zoom);

		SDL_FRect topR = { static_cast<float>(def->top.x), static_cast<float>(def->top.y), static_cast<float>(def->top.w), static_cast<float>(def->top.h) };
		renderer->drawTexture(tex, dx, dy, 100.0 * zoom, 100.0 * zoom, 0, nullptr, SDL_FLIP_NONE, &topR);

		if (def->hasSide)
		{
			int32_t sdw = std::lround(def->side.w * zoom);
			int32_t sdh = std::lround(def->side.h * zoom);
			int32_t sdx = screenX - sdw / 2;
			int32_t sdy = dy + dh;
			SDL_FRect sideR = { static_cast<float>(def->side.x), static_cast<float>(def->side.y), static_cast<float>(def->side.w), static_cast<float>(def->side.h) };
			renderer->drawTexture(tex, sdx, sdy, 100.0 * zoom, 100.0 * zoom, 0, nullptr, SDL_FLIP_NONE, &sideR);
		}
		return;
	}

	if (m_sprite == nullptr)
		return;

	auto* t = GetComponent<IETransformComponent>();
	if (t == nullptr)
		return;

	m_sprite->draw(
		renderer,
		screenX,
		screenY,
		static_cast<double>(t->GetScaleX()),
		static_cast<double>(t->GetScaleY()),
		static_cast<double>(t->GetRotation()));
}
