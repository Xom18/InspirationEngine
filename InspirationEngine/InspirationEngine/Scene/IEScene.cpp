#include "InspirationEngine.h"

IEScene::~IEScene() = default;

void IEScene::Update(float deltaTime)
{
	for (auto& obj : m_objects)
	{
		if (obj->IsActive())
			obj->Update(deltaTime);
	}
}

void IEScene::Draw(IERenderer* renderer)
{
	if (m_camera != nullptr)
	{
		int32_t vw = renderer->m_logicalWidth  > 0 ? renderer->m_logicalWidth  : renderer->m_w;
		int32_t vh = renderer->m_logicalHeight > 0 ? renderer->m_logicalHeight : renderer->m_h;
		m_camera->SetViewport(vw, vh);
	}

	std::vector<IEGameObject*> sorted;
	sorted.reserve(m_objects.size());
	for (auto& obj : m_objects)
	{
		if (obj->IsActive())
			sorted.push_back(obj.get());
	}

	if (m_camera != nullptr)
	{
		std::sort(sorted.begin(), sorted.end(), [this](const IEGameObject* a, const IEGameObject* b)
		{
			auto* ta = a->GetComponent<IETransformComponent>();
			auto* tb = b->GetComponent<IETransformComponent>();
			float ka = ta != nullptr ? m_camera->GetSortKey(ta->GetX(), ta->GetY(), ta->GetZ()) : 0.0f;
			float kb = tb != nullptr ? m_camera->GetSortKey(tb->GetX(), tb->GetY(), tb->GetZ()) : 0.0f;
			return ka < kb;
		});
	}
	else
	{
		std::sort(sorted.begin(), sorted.end(), [](const IEGameObject* a, const IEGameObject* b)
		{
			return a->GetSortKey() < b->GetSortKey();
		});
	}

	for (auto* obj : sorted)
	{
		auto* t = obj->GetComponent<IETransformComponent>();

		int32_t sx, sy;
		if (m_camera != nullptr && t != nullptr)
		{
			auto pos = m_camera->WorldToScreen(t->GetX(), t->GetY(), t->GetZ());
			sx = pos.m_x;
			sy = pos.m_y;
		}
		else
		{
			sx = t != nullptr ? static_cast<int32_t>(t->GetX()) : 0;
			sy = t != nullptr ? static_cast<int32_t>(t->GetY()) : 0;
		}

		obj->Draw(renderer, sx, sy, m_camera != nullptr ? m_camera->GetZoom() : 1.0f);
	}
}

void IEScene::RemoveObject(IEGameObject* target)
{
	auto it = std::find_if(m_objects.begin(), m_objects.end(),
		[target](const std::unique_ptr<IEGameObject>& p) { return p.get() == target; });
	if (it != m_objects.end())
		m_objects.erase(it);
}
