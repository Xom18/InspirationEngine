#include "InspirationEngine.h"

IEScene::~IEScene() = default;

void IEScene::update(float deltaTime)
{
	for (auto& obj : m_objects)
		if (obj->isActive()) obj->update(deltaTime);
}

void IEScene::draw(IERenderer* renderer)
{
	std::vector<IEGameObject*> sorted;
	sorted.reserve(m_objects.size());
	for (auto& obj : m_objects)
		if (obj->isActive()) sorted.push_back(obj.get());

	std::sort(sorted.begin(), sorted.end(), [](const IEGameObject* a, const IEGameObject* b) {
		return a->getSortKey() < b->getSortKey();
	});

	for (auto* obj : sorted)
		obj->draw(renderer);
}

void IEScene::removeObject(IEGameObject* target)
{
	auto it = std::find_if(m_objects.begin(), m_objects.end(),
		[target](const std::unique_ptr<IEGameObject>& p) { return p.get() == target; });
	if (it != m_objects.end())
		m_objects.erase(it);
}
