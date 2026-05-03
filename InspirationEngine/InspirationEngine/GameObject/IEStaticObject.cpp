#include "InspirationEngine.h"

void IEStaticObject::draw(IERenderer* renderer, int32_t screenX, int32_t screenY, float zoom)
{
	if (!m_active || renderer == nullptr)
		return;

	auto* tc = getComponent<IETileComponent>();
	if (tc != nullptr)
	{
		const IETileDef* def = IECore::m_Atlas.getTile(tc->atlas, tc->tile);
		SDL_Texture*     tex = IECore::m_Atlas.getTexture(tc->atlas);
		if (def == nullptr || tex == nullptr) return;

		int32_t dw = static_cast<int32_t>(def->top.w * zoom);
		int32_t dh = static_cast<int32_t>(def->top.h * zoom);
		int32_t dx = screenX - static_cast<int32_t>(dw * def->anchorX);
		int32_t dy = screenY - static_cast<int32_t>(dh * def->anchorY);
		SDL_Rect topR = { def->top.x, def->top.y, def->top.w, def->top.h };
		renderer->drawTexture(tex, dx, dy, 100.0 * zoom, 100.0 * zoom, 0, nullptr, SDL_FLIP_NONE, &topR);

		if (def->hasSide)
		{
			int32_t sdw = static_cast<int32_t>(def->side.w * zoom);
			int32_t sdh = static_cast<int32_t>(def->side.h * zoom);
			int32_t sdx = screenX - sdw / 2;
			int32_t sdy = dy + dh;
			SDL_Rect sideR = { def->side.x, def->side.y, def->side.w, def->side.h };
			renderer->drawTexture(tex, sdx, sdy, 100.0 * zoom, 100.0 * zoom, 0, nullptr, SDL_FLIP_NONE, &sideR);
		}
		return;
	}

	if (m_sprite == nullptr) return;

	auto* t = getComponent<IETransformComponent>();
	if (t == nullptr)
		return;

	m_sprite->draw(
		renderer,
		screenX,
		screenY,
		static_cast<double>(t->scaleX),
		static_cast<double>(t->scaleY),
		static_cast<double>(t->rotation));
}
