#include "InspirationEngine.h"

IEScene::~IEScene() = default;

void IEScene::update(float deltaTime)
{
	for (auto& obj : m_objects)
		if (obj->isActive()) obj->update(deltaTime);
}

void IEScene::draw(IERenderer* renderer)
{
	// 카메라 뷰포트를 렌더러 논리 해상도에 맞춤
	if (m_camera != nullptr)
	{
		int32_t vw = renderer->m_logicalWidth  > 0 ? renderer->m_logicalWidth  : renderer->m_w;
		int32_t vh = renderer->m_logicalHeight > 0 ? renderer->m_logicalHeight : renderer->m_h;
		m_camera->setViewport(vw, vh);
	}

	// 활성 오브젝트 수집
	std::vector<IEGameObject*> sorted;
	sorted.reserve(m_objects.size());
	for (auto& obj : m_objects)
		if (obj->isActive()) sorted.push_back(obj.get());

	// 정렬 — 카메라가 있으면 카메라 sortKey, 없으면 오브젝트 자체 sortKey
	if (m_camera != nullptr)
	{
		std::sort(sorted.begin(), sorted.end(), [this](const IEGameObject* a, const IEGameObject* b)
		{
			auto* ta = a->getComponent<IETransformComponent>();
			auto* tb = b->getComponent<IETransformComponent>();
			float ka = ta != nullptr ? m_camera->getSortKey(ta->x, ta->y, ta->z) : 0.0f;
			float kb = tb != nullptr ? m_camera->getSortKey(tb->x, tb->y, tb->z) : 0.0f;
			return ka < kb;
		});
	}
	else
	{
		std::sort(sorted.begin(), sorted.end(), [](const IEGameObject* a, const IEGameObject* b)
		{
			return a->getSortKey() < b->getSortKey();
		});
	}

	// 드로우 — 카메라가 있으면 worldToScreen 투영, 없으면 월드 좌표 직접 사용
	for (auto* obj : sorted)
	{
		auto* t = obj->getComponent<IETransformComponent>();

		int32_t sx, sy;
		if (m_camera != nullptr && t != nullptr)
		{
			auto pos = m_camera->worldToScreen(t->x, t->y, t->z);
			sx = pos.m_x;
			sy = pos.m_y;
		}
		else
		{
			sx = t != nullptr ? static_cast<int32_t>(t->x) : 0;
			sy = t != nullptr ? static_cast<int32_t>(t->y) : 0;
		}

		obj->draw(renderer, sx, sy);
	}
}

void IEScene::removeObject(IEGameObject* target)
{
	auto it = std::find_if(m_objects.begin(), m_objects.end(),
		[target](const std::unique_ptr<IEGameObject>& p) { return p.get() == target; });
	if (it != m_objects.end())
		m_objects.erase(it);
}
