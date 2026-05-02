#include "InspirationEngine.h"

void IEEntity::update(float deltaTime)
{
	auto* t = getComponent<IETransformComponent>();
	auto* v = getComponent<IEVelocityComponent>();
	if (t == nullptr || v == nullptr)
		return;

	t->x += v->vx * deltaTime;
	t->y += v->vy * deltaTime;
	t->z += v->vz * deltaTime;
}

void IEEntity::draw(IERenderer* renderer)
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
