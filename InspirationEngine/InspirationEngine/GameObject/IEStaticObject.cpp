#include "InspirationEngine.h"

void IEStaticObject::draw(IERenderer* renderer, int32_t screenX, int32_t screenY)
{
	if (!m_active || m_sprite == nullptr || renderer == nullptr)
		return;

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
