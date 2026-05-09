#include "InspirationEngine.h"

void IEDebugInfo::DrawOverlay(IERenderer* renderer, IEScene* scene, IEFont* font)
{
	if (renderer == nullptr)
		return;

	IECamera* camera = scene ? scene->GetCamera() : nullptr;

	DrawGrid(renderer, camera);
	if (scene)
		DrawObjectMarkers(renderer, scene, camera, font);
	DrawCameraInfo(renderer, camera, font);
}

void IEDebugInfo::DrawGrid(IERenderer* renderer, IECamera* camera)
{
	SDL_Color gridColor = { 80, 80, 80, 128 };

	int32_t vw = renderer->GetW();
	int32_t vh = renderer->GetH();

	if (camera == nullptr)
	{
		for (int32_t x = 0; x < vw; x += 64)
			renderer->drawLine(gridColor, x, 0, x, vh);
		for (int32_t y = 0; y < vh; y += 64)
			renderer->drawLine(gridColor, 0, y, vw, y);
		return;
	}

	const int32_t STEP  = 64;
	const int32_t RANGE = 16;

	int32_t baseX = (std::lround(camera->GetX()) / STEP) * STEP;
	int32_t baseY = (std::lround(camera->GetY()) / STEP) * STEP;
	int32_t startX = baseX - RANGE * STEP;
	int32_t startY = baseY - RANGE * STEP;
	int32_t endX   = baseX + RANGE * STEP;
	int32_t endY   = baseY + RANGE * STEP;

	for (int32_t wx = startX; wx <= endX; wx += STEP)
	{
		auto p0 = camera->WorldToScreen(static_cast<float>(wx), static_cast<float>(startY));
		auto p1 = camera->WorldToScreen(static_cast<float>(wx), static_cast<float>(endY));
		renderer->drawLine(gridColor, p0.m_x, p0.m_y, p1.m_x, p1.m_y);
	}
	for (int32_t wy = startY; wy <= endY; wy += STEP)
	{
		auto p0 = camera->WorldToScreen(static_cast<float>(startX), static_cast<float>(wy));
		auto p1 = camera->WorldToScreen(static_cast<float>(endX), static_cast<float>(wy));
		renderer->drawLine(gridColor, p0.m_x, p0.m_y, p1.m_x, p1.m_y);
	}
}

void IEDebugInfo::DrawObjectMarkers(IERenderer* renderer, IEScene* scene, IECamera* camera, IEFont* font)
{
	SDL_Color markerColor = { 255, 100,   0, 200 };
	SDL_Color textColor   = { 255, 220,   0, 255 };
	const int32_t CROSS = 6;

	for (const auto& obj : scene->GetObjects())
	{
		if (!obj->IsActive())
			continue;

		auto* t = obj->GetComponent<IETransformComponent>();
		if (t == nullptr)
			continue;

		int32_t sx, sy;
		if (camera != nullptr)
		{
			auto pos = camera->WorldToScreen(t->GetX(), t->GetY(), t->GetZ());
			sx = pos.m_x;
			sy = pos.m_y;
		}
		else
		{
			sx = static_cast<int32_t>(t->GetX());
			sy = static_cast<int32_t>(t->GetY());
		}

		renderer->drawLine(markerColor, sx - CROSS, sy, sx + CROSS, sy);
		renderer->drawLine(markerColor, sx, sy - CROSS, sx, sy + CROSS);

		if (font != nullptr)
		{
			char buf[64];
			snprintf(buf, sizeof(buf), "(%.0f,%.0f,%.0f)", t->GetX(), t->GetY(), t->GetZ());
			renderer->drawText(font, buf, textColor, sx + CROSS + 2, sy - 8);
		}
	}
}

void IEDebugInfo::DrawCameraInfo(IERenderer* renderer, IECamera* camera, IEFont* font)
{
	if (font == nullptr)
		return;

	SDL_Color textColor = { 0, 255, 128, 255 };
	char buf[128];

	if (camera != nullptr)
	{
		snprintf(buf, sizeof(buf), "CAM (%.1f, %.1f, %.1f) zoom:%.2f vp:%dx%d",
			camera->GetX(), camera->GetY(), camera->GetZ(),
			camera->GetZoom(),
			camera->GetViewportWidth(), camera->GetViewportHeight());
	}
	else
	{
		snprintf(buf, sizeof(buf), "CAM: none");
	}

	renderer->drawText(font, buf, textColor, 4, 4);
}
