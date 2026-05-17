#include "InspirationEngine.h"

void IEStaticObject::Draw(IERenderer* renderer, int32_t screenX, int32_t screenY, float zoom)
{
	if (!IsActive() || renderer == nullptr)
		return;

	auto* tc = GetComponent<IETileComponent>();
	if (tc != nullptr)
	{
		const IETileDef* def = IECore::GetAtlas().GetTile(tc->GetAtlas(), tc->GetTile());
		SDL_Texture*     tex = IECore::GetAtlas().GetTexture(tc->GetAtlas());
		if (def == nullptr || tex == nullptr)
			return;

		int32_t dh = std::lround(def->top.h * zoom);
		int32_t dx = screenX - std::lround(def->pivotX * zoom);
		int32_t dy = screenY - std::lround(def->pivotY * zoom);

		SDL_FRect topR = { static_cast<float>(def->top.x), static_cast<float>(def->top.y), static_cast<float>(def->top.w), static_cast<float>(def->top.h) };
		renderer->DrawTexture(tex, dx, dy, 100.0 * zoom, 100.0 * zoom, 0, nullptr, SDL_FLIP_NONE, &topR);

		if (def->hasSide)
		{
			int32_t sdw = std::lround(def->side.w * zoom);
			int32_t sdx = screenX - sdw / 2;
			int32_t sdy = dy + dh;
			SDL_FRect sideR = { static_cast<float>(def->side.x), static_cast<float>(def->side.y), static_cast<float>(def->side.w), static_cast<float>(def->side.h) };
			renderer->DrawTexture(tex, sdx, sdy, 100.0 * zoom, 100.0 * zoom, 0, nullptr, SDL_FLIP_NONE, &sideR);
		}
		return;
	}

	if (GetSprite() == nullptr)
		return;

	auto* t = GetComponent<IETransformComponent>();
	if (t == nullptr)
		return;

	GetSprite()->Draw(
		renderer,
		screenX,
		screenY,
		static_cast<double>(t->GetScaleX()),
		static_cast<double>(t->GetScaleY()),
		static_cast<double>(t->GetRotation()));
}
