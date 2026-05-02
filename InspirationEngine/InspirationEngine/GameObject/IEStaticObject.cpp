#include "InspirationEngine.h"

void IEStaticObject::draw(IERenderer* renderer)
{
	if (!m_active || m_sprite == nullptr || renderer == nullptr)
		return;

	auto* t = getComponent<IETransformComponent>();
	if (t == nullptr)
		return;

	m_sprite->draw(
		renderer,
		static_cast<int32_t>(t->x),
		static_cast<int32_t>(t->y),
		static_cast<double>(t->scaleX),
		static_cast<double>(t->scaleY),
		static_cast<double>(t->rotation));
}
