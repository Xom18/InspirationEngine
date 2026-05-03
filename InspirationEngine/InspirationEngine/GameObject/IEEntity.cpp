#include "InspirationEngine.h"

void IEEntity::Update(float deltaTime)
{
	auto* t = GetComponent<IETransformComponent>();
	auto* v = GetComponent<IEVelocityComponent>();
	if (t == nullptr || v == nullptr)
		return;

	t->SetX(t->GetX() + v->GetVx() * deltaTime);
	t->SetY(t->GetY() + v->GetVy() * deltaTime);
	t->SetZ(t->GetZ() + v->GetVz() * deltaTime);
}

void IEEntity::Draw(IERenderer* renderer, int32_t screenX, int32_t screenY, float zoom)
{
	if (!m_active || m_sprite == nullptr || renderer == nullptr)
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
